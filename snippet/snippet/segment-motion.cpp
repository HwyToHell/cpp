#include "stdafx.h"
#include <climits>
#pragma warning(disable: 4482) // MSVC10: enum nonstd extension

#include "../../../video-dev/car-count/include/config.h"
#include "../../../video-dev/car-count/include/frame_handler.h"
#include "../../../video-dev/car-count/include/tracker.h"
#include "../../../video-dev/car-count/include/recorder.h"
//#include "../../inc/program_options.h"
#include "../../inc/opencv/backgroundsubtraction.h"

using namespace std;

cv::Mat getCrossKernel(int kernelSize, double percent);
cv::Mat getCrossKernel(int kernelSize, int crossArmSize);



class Segmentation {
public:
	Segmentation(int blobAreaMin, int blobAreaMax);
	std::vector<cv::Rect>	getBBoxes(cv::Mat& mask);
	double					getThreshold();
	cv::Mat					segmentFrame(cv::Mat& frame);
	void					showBboxes(cv::Mat& frame);
	void					showContours(cv::Mat& frame);
	void					showHulls(cv::Mat& frame);
private:
	cv::Ptr<cv::BackgroundSubtractorMOG2>	m_mog2;
	int										m_blobAreaMin;
	int										m_blobAreaMax;
	std::vector<cv::Rect>					m_bboxes;
	std::vector<std::vector<cv::Point> >	m_contours;
	std::vector<cv::Vec4i>					m_hierarchy;
	std::vector<std::vector<cv::Point> >	m_hulls;
};






boost::mutex mtxImage;
boost::mutex mtxLabel;

boost::condition_variable cndImage;
bool isImageAvailable;
bool isSourceFeeding;
cv::Mat srcImage;

int label;


// classification thread: wait for new image, wake thread, lock srcImage when reading
void waitForNewImage(cv::Mat& srcImage, cv::Mat& dstImage) {
	boost::unique_lock<boost::mutex> lock(mtxImage);
	while(!isImageAvailable) {
		cndImage.wait(lock);
	}
	dstImage = srcImage.clone();
	isImageAvailable = false;
	cndImage.notify_one();
	return;
}

// main thread: wait for image buffer to be available
void waitForImageBuffer(cv::Mat& image, cv::Rect roi, cv::Mat& imgBuf) {
	boost::unique_lock<boost::mutex> lock(mtxImage);
	while(isImageAvailable) {
		cndImage.wait(lock);
	}
	imgBuf = image(roi);
	isImageAvailable = true;
	cndImage.notify_one();
	return;
}

// write classification results to other thread
void writeClassification(int classLabel) {
	mtxLabel.lock();
	classLabel = 1;
	mtxLabel.unlock();
	return;
}


// thread
void classify(cv::Mat& srcImage) {
	boost::chrono::seconds duration(1);
	cv::Mat blob;

	while(isSourceFeeding) {

		waitForNewImage(srcImage, blob);

		for (int i = 1; i < 10; ++i) {
			using namespace boost::this_thread;
			sleep_for(duration);
			cout << "classifying for " << i << " sec" << endl;
		}
		cout << "finished" << endl << endl;

		// writeClassification(label);
	}
}

/// return vector of rectangles indentifying motion objects
/// input: foreground mask, kernel size for dilation, minimal blob area for combining blobs
/// process: dilate foreground mask, combine larger blobs, remove smaller blobs
// in post-process.cpp
std::vector<cv::Rect> extractMotionRects(cv::Mat& foregroundMask, int kernelSize, int minBlobArea, cv::Mat* debugImage);

/// shows actual and previous track entry in track image
void printTrackUpdate(cv::Mat trackImage, std::list<Track>* pDebugTracks);


int main(int argc, char* argv[]) {
	isSourceFeeding = true;
	isImageAvailable = false;
	int label = 0;
	cout << "Hello Threads!" << endl;

	// video from file
	cv::VideoCapture cap;
	string videoPath("D:\\Users\\Holger\\counter\\traffic640x480.avi");
	cap.open(videoPath);
	cv::Mat image;

	// roi
	cv::Rect roi(250, 200, 200, 200);
	cv::Mat srcImage;

	// status variables	
	bool isVehicle = false;
	long long frameCount = 0;
	
	// TODO UNCOMMENT for classification
	//boost::thread classifyThread(classify, srcImage);

	BackgroundSubtractorVibe vibe;
	
	Config config;
	Config* pConfig = &config;

	// collection of tracks and vehicles, generated by motion detection in frame_handler
	SceneTracker scene(pConfig); 
	SceneTracker* pScene = &scene;
	config.attach(pScene);

	// DEBUG
	std::list<Track>* pDebugTracks;

	while (cap.read(image)) {

		++frameCount;
		cv::rectangle(image, roi, green);
		cv::putText(image, to_string(frameCount), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, green, 2);
		//cv::imshow("frame", image);

		// apply roi
		cv::Mat frame_roi = image(roi);
		cv::imshow("frame 200x200", frame_roi);

		// vibe rgb background subtraction
		//   original size 200x200
		cv::Mat vibemask;
		cv::Mat frame_roi_smooth;
		cv::blur(frame_roi, frame_roi_smooth, cv::Size(5,5));
		vibe.apply(frame_roi_smooth, vibemask);
		//cv::imshow("vibe 200x200", vibemask);

		// post-processing
		//// remove "salt-and-pepper" noise
		cv::Mat highResMask, lowResMask;
		cv::medianBlur(vibemask, lowResMask, 7);

		//// dilate and calculate bounding boxes around motion objects
		int dilateKernel = 21;
		int minBlobArea = 702;
		cv::Mat debugImg(frame_roi.size(), CV_8UC3, black);

		std::vector<cv::Rect> motionRects = extractMotionRects(lowResMask, 
			dilateKernel, minBlobArea, &debugImg);
		cv::imshow("debug", debugImg);

		// vector -> list (required input for SceneTracker.updateTracks);
		std::list<TrackEntry> motionRectList;
		for (unsigned int i = 0; i < motionRects.size(); ++i) {
			motionRectList.push_back(motionRects[i]);
		}

		pDebugTracks = scene.updateTracks(motionRectList);

		// DEBUG print tracking results
		cv::Mat trackImg(frame_roi.size(),  CV_8UC3, black);
		printTrackUpdate(trackImg, pDebugTracks);
		cv::imshow("tracker", trackImg);




		std::vector<TrackEntry> countedRects = scene.triggerDNNClassif();

		/*
		// bbox with large masked high res segmentation result
		cv::medianBlur(vibemask, highResMask, 3);
		cv::Rect bbox = bboxFromHighResForeGroundMask(highResMask, largeMask);
		*/



		// create masked frame in color
		cv::Mat maskedFrame, largeMask;
		//   create large mask from bounding boxes of motion objects
		for (unsigned int i = 0; i < motionRects.size(); ++i) { 
			cv::rectangle(largeMask, motionRects[i], white, CV_FILLED);
		}
		frame_roi.copyTo(maskedFrame, largeMask);
		cv::putText(maskedFrame, to_string(frameCount), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, green, 2);
		cv::imshow("moving objects", maskedFrame);


		
		// DEBUG frame sequence of interest
		// save segmentation results to image files
		//if (countedRects.size() > 0) {
		if (frameCount >= 350 && frameCount <= 400) {
			cout << "frame: " << frameCount << endl;

			// save debug image
			string path = "D:\\Users\\Holger\\counter\\segment-motion\\";
			string filePath = path + "debug_" + to_string(frameCount) + ".png";
			cv::imwrite(filePath, debugImg);

			//filePath = path + "masked_frame" + to_string(frameCount) + ".png";
			//cv::imwrite(filePath, maskedFrame);

			filePath = path + "track_" + to_string(frameCount) + ".png";
			cv::imwrite(filePath, trackImg);
		}



			
		// TODO UNCOMMENT for classification
		/*
		// make up vehicle detection
		int remainder = frameCount % 50;
		if (!remainder)
			isVehicle = true;

		// vehicle detected
		if (isVehicle) {
			// wait for image buffer available
			waitForImageBuffer(image, roi, srcImage);
			isVehicle = false;
		}
		*/

				
		// break loop with ESC
		if (cv::waitKey(10) == 27) 	{
			cout << "ESC pressed -> end video processing" << endl;
			//cv::imwrite("frame.jpg", frame);
			break;
		}
	}

	/*/ notify about new data
	mtxImage.lock();
	srcImage = image(roi);
	isImageAvailable = true;
	mtxImage.unlock();
	*/

	cout << "thread finished, class: " << label << endl;

	isSourceFeeding = false;
	// TODO UNCOMMENT for classification
	//classifyThread.join();

	cout << endl << "Press <enter> to exit" << endl;
	string str;
	getline(cin, str);
	
	return 0;
}



std::vector<TrackEntry>SceneTracker::triggerDNNClassif() {
	using namespace std;

	const int minConfidence = 4;
	const int minTrackLength = 10;
	const int roiWidth = 200;

	// for all tracks:
	//   track confidence high?
	//	 track length > counting_limit?
	//   centroid near mid of window?
	// 
	
	vector<TrackEntry> countedObjects;

	std::list<Track>::iterator iTrack = mTracks.begin();
	while (iTrack != mTracks.end()) {
		int confidence = iTrack->getConfidence();
		int trackLength = iTrack->getLength();

		// TODO  more than one motion object in scene -> change counting positions

		
		// consider only tracks that have not been counted yet
		// with reliable motion detections
		//    !counted
		//    >= minConfidence:		supresses random motion detections
		//    >= minTrackLength:	suppresses motion detections that don't move
		//							e.g. moving leaves and tree branches
		if ( (!iTrack->isCounted()) 
			&& (confidence >= minConfidence) 
			&& (trackLength >= minTrackLength) ) {

			int vehicleCentroidX		 = iTrack->getActualEntry().centroid().x;
			int vehicleLength	 = iTrack->getActualEntry().width();
			cv::Point2d velocity = iTrack->getVelocity();


			if (signBit(velocity.x)) { // moving to left

				// vehicle centroid must have more than 1/3 distance from right border

				int countPosLeft = roiWidth * 2 / 3; 
				if (vehicleCentroidX < countPosLeft) {
					countedObjects.push_back(iTrack->getActualEntry());
					iTrack->setCounted(true);
					cout << "to left,  length: " << vehicleLength << endl; 
				}

			} else { // moving to right

				// vehicle centroid must have more than 1/3 distance from left border
				int countPosRight = roiWidth / 3; 
				if (vehicleCentroidX > countPosRight) {
					countedObjects.push_back(iTrack->getActualEntry());
					iTrack->setCounted(true);
					cout << "to right, length: " << vehicleLength << endl; 
				}

			}

		} // end_if count only reliable detections

		++iTrack;
	} // end_while

	
	return countedObjects;
}


Segmentation::Segmentation(int blobAreaMin, int blobAreaMax) :
	m_blobAreaMin(blobAreaMin),
	m_blobAreaMax(blobAreaMax) {
	m_mog2 = cv::createBackgroundSubtractorMOG2(100, 25, false);
}


std::vector<cv::Rect> Segmentation::getBBoxes(cv::Mat& mask) {
	std::vector<cv::Rect> bboxes;

	// find contours
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, m_contours, m_hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
	
	
	// DEBUG
	//cout << "contours: " << m_contours.size() << endl;
	//cout << "hierarchy: " << m_hierarchy.size() << endl << endl;

	// calc bounding boxes, check limits
	// calc bounding boxes of all detections and store in m_bBoxes
	m_bboxes.clear();
	m_hulls.clear();
	for (unsigned int i = 0; i < m_contours.size(); i++) { 
		// convex hull of contour
		std::vector<cv::Point> hull;
		cv::convexHull(m_contours[i], hull);
		cv::Rect bbox = cv::boundingRect(hull);
		// DEBUG
		// cout << "  bbox: " << bbox << endl;
		if ((bbox.area() > m_blobAreaMin) && (bbox.area() < m_blobAreaMax)) {
				m_bboxes.push_back(bbox);
				m_hulls.push_back(hull);
		}
	}

	return bboxes;
}

double Segmentation::getThreshold() {
	return m_mog2->getVarThreshold();
}


cv::Mat Segmentation::segmentFrame(cv::Mat& frame) {
	//cv::blur(frame, frame, cv::Size(12, 12));
	cv::Mat mask;
	//cv::medianBlur(frame, frame, 7);
	m_mog2->apply(frame, mask);
	//cv::medianBlur(mask, mask, 3);
	//cv::Mat kernel = cv::getStructuringElement(cv::MorphShapes::MORPH_CROSS, cv::Size(11,11));
	//cv::Mat kernel = getCrossKernel(13, 3);
	// DEBUG 
	// cout << "kernel: " << endl << kernel << endl;

	//cv::morphologyEx(mask, mask, cv::MorphTypes::MORPH_DILATE, kernel);
	//cv::dilate(mask, mask, cv::Mat(11,11,CV_8UC1));
	//cv::erode(mask, mask, cv::Mat(15,15,CV_8UC1));
	return mask;
	//return frame;
}

void Segmentation::showBboxes(cv::Mat& frame) {
    for (int i = 0; i < m_bboxes.size(); ++i) {
		cv::rectangle(frame, m_bboxes.at(i), green);
	}
}

void Segmentation::showContours(cv::Mat& frame) {
	for (int i = 0; i < m_hierarchy.size(); ++i) 
		cv::drawContours(frame, m_contours, i, red);
	return;
}


void Segmentation::showHulls(cv::Mat& frame) {
	for (int i = 0; i < m_hulls.size(); ++i) 
		cv::drawContours(frame, m_hulls, i, yellow);
	return;
}


void printTrackUpdate(cv::Mat trackImage, std::list<Track>* pDebugTracks) {
	cv::Scalar color[4] = {blue, green, red, yellow};
	int idx = 0;

	std::list<Track>::iterator iTrack = pDebugTracks->begin();
	while (iTrack != pDebugTracks->end()) {
		int confidence = iTrack->getConfidence();
		int id = iTrack->getId();
		int length = iTrack->getLength();

		cv::Rect rcActual = iTrack->getActualEntry().rect();
		cv::rectangle(trackImage, rcActual, color[id % 4], 2);
		
		cv::Rect rcPrev = iTrack->getPreviousEntry().rect();
		cv::rectangle(trackImage, rcPrev, color[id % 4]);

		std::stringstream ss;
		int xOffset = 5;
		int yOffset = 10 + idx * 10;
		ss << "#" << id << ", conf: " << confidence << ", len: " << length;
		cv::putText(trackImage, ss.str(), cv::Point(xOffset, yOffset),
			cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 0.4, color[id % 4]);

		++idx;
		++iTrack;
	}
}



// --- support functions ------------------------------------------------------
cv::Mat getCrossKernel(int kernelSize, int crossArmSize) {
	cv::Mat kernel(kernelSize, kernelSize, CV_8UC1);

	if (crossArmSize < 1) {
		crossArmSize = 1;
		cout << "crossArmSize set to 1, must be larger than 0" << endl;
	}

	if (crossArmSize > kernelSize) {
		crossArmSize = kernelSize;
		cout << "crossArmSize set to kernelSize, must be smaller or equal to kernelSize" << endl;
	}

	int colSetStart = (kernelSize - crossArmSize) / 2;
	int rowSetStart = (kernelSize - crossArmSize) / 2;
	int colSetStop = colSetStart + crossArmSize;
	int rowSetStop = rowSetStart + crossArmSize;

	int row, col;
	for (row = 0; row < kernelSize; ++row) {
		unsigned char* ptr = kernel.ptr(row);
		
		// row outside cross arm
		if (row < rowSetStart || row >= rowSetStop)
			for (col = 0; col < kernelSize; ++col) 
				// column outside cross arm
				if (col < colSetStart || col >= colSetStop) 
					ptr[col] = 0;
				// inside cross arm
				else 
					ptr[col] = 1;
		// row inside cross arm
		else
			for (col = 0; col < kernelSize; ++col) 
					ptr[col] = 1;
	}

	return kernel;
}


cv::Mat getCrossKernel(int kernelSize, double percent) {
	double crossArmWidthDouble = static_cast<double> (kernelSize * percent) / 100;
	int crossArmWidth = static_cast<int> (round (crossArmWidthDouble));

	return getCrossKernel(kernelSize, crossArmWidth);
}


