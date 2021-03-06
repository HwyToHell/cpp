#include "stdafx.h"

#include "../../cpp/inc/id_pool.h"
#include "../../cpp/inc/pick_list.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/frame_handler.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/tracker.h"


/// pause console until <enter> has been pressed
bool waitForEnter();

typedef std::vector<std::list<TrackState>> TrackStateVec;
TrackStateVec	g_trackState;
size_t			g_idx = 0;

struct MovingBlob {
	cv::Point	origin;
	cv::Size	size;
	cv::Point	velocity;
	MovingBlob(const cv::Point& origin_, const cv::Size& size_, const cv::Point& velocity_)
		: origin(origin_), size(size_), velocity(velocity_) {};
};

typedef std::vector<std::list<cv::Rect>> BlobTimeSeries;

typedef std::vector<std::list<Track>> TrackTimeSeries;

typedef std::list<Occlusion> Occlusions;


BlobTimeSeries moveThroughRoi(const cv::Size roi, const MovingBlob& right, const MovingBlob& left);

cv::Rect clipAtRoi(cv::Rect rec, cv::Size roi) {
	cv::Rect rcRoi(cv::Point(0,0), roi);
	return rec & rcRoi;
}

void createOcclusion(IdGen* pIds, cv::Size roi, Track* trRight, Track* trLeft) {
	using namespace std;
	int steps = 10;
	Occlusion occ1(roi, trRight, trLeft, steps); 
	cout << "Occlusion 1 ID:" << occ1.id() << endl;

	Occlusion occ2(roi, trRight, trLeft, steps); 
	cout << "Occlusion 2 ID:" << occ2.id() << endl;
}


BlobTimeSeries createOcclusion(const cv::Size roi, const cv::Size blob) {
	// moves to left
	cv::Point endOrgLeft(70, 70);
	cv::Rect endLeft(endOrgLeft, blob);
	cv::Point veloLeft(-5,0);

	// moves to right		
	int gapX(40);
	cv::Point endOrgRight(endOrgLeft.x - gapX - blob.width, 70);
	cv::Rect endRight(endOrgRight, blob);
	cv::Point veloRight(5, 0);

	MovingBlob right(endOrgRight, blob, veloRight);
	MovingBlob left(endOrgLeft, blob, veloLeft);
	BlobTimeSeries blobTimeSeries = moveThroughRoi(roi, right, left);

	return blobTimeSeries;
}


/// create track from constant velocity
Track createTrack(cv::Size roi, cv::Rect end, cv::Point velocity, int steps, int id = 0) {
	Track track;
	cv::Rect rc(end.tl() - velocity * (steps - 1), end.size());
	for (int i = 0; i < steps; ++i) {
		// addTrackEntry modifies rc, if out of roi, thus copy needed
		cv::Rect rcAdd = rc; 
		track.addTrackEntry(rcAdd, roi);
		rc += velocity;
	}
	return track;
}


BlobTimeSeries moveThroughRoi(const cv::Size roi, const MovingBlob& right, const MovingBlob& left) {
	using namespace std;
	vector<list<cv::Rect>> timeSeries;

	// check velocity direction
	if (right.velocity.x <= 0) {
		cout << "velocity to right must be greater than 0" << endl;
		return timeSeries;
	}
	if (left.velocity.x >= 0) {
		cout << "velocity to left must be less than 0" << endl;
		return timeSeries;
	}

	// check blob inside roi
	cv::Rect roiRect(cv::Point(0,0), roi);
	cv::Rect rightRect(right.origin, right.size);
	if ( (roiRect & rightRect).empty() ) {
		cout << "blob right: " << rightRect << " outside roi" << endl;
		return timeSeries;
	}
	cv::Rect leftRect(left.origin, left.size);
	if ( (roiRect & leftRect).empty() ) {
		cout << "blob left: " << leftRect << " outside roi" << endl;
		return timeSeries;
	}

	// update steps to go through roi in x coord
	int nRight = (roi.width - right.origin.x) / right.velocity.x;
	int nLeft = (left.origin.x + left.size.width) / std::abs(left.velocity.x);
	int n = nRight > nLeft ? nRight : nLeft;

	// push blobs to list for all update steps
	for (; n >=0; --n) {
		list<cv::Rect> blobs;

		// combine rects, if intersection
		// clip rects, if outside roi
		cv::Rect intersection = rightRect & leftRect;
		if ( intersection.empty() ) {
			blobs.push_back(clipAtRoi(rightRect, roi));
			blobs.push_back(clipAtRoi(leftRect, roi));
		} else {
			blobs.push_back(clipAtRoi(rightRect | leftRect, roi));
		}
		
		timeSeries.push_back(blobs);
		rightRect += right.velocity;
		leftRect += left.velocity;
	}
	return timeSeries;
}


void printBlobsAt(cv::Mat& canvas, const BlobTimeSeries& timeSeries, const size_t idx) {
		using namespace std;
		// check upper index bound
		size_t idxTime = (idx >= timeSeries.size()) ? timeSeries.size()-1 : idx;
		cv::Scalar color[3] = {red, green, yellow};
		size_t idxColor = 0;

		// loop through available blobs
		list<cv::Rect>::const_iterator iBlob = timeSeries[idx].begin();
		while (iBlob != timeSeries[idx].end()) {
			cv::rectangle(canvas, *iBlob, color[idxColor]);
			++iBlob;
		}

		return;
}


void printTracksAt(cv::Mat& canvas, const TrackTimeSeries& timeSeries, const size_t idxUnchecked) {
	// appearance
	cv::Scalar color[] = {blue, green, red, yellow};
	size_t nColors = sizeof(color) / sizeof(color[0]);

	// check upper index bound
	size_t idx = (idxUnchecked >= timeSeries.size()) ? timeSeries.size()-1 : idxUnchecked;

	// loop through available tracks
	std::list<Track>::const_iterator iTrack = timeSeries[idx].begin();
	while (iTrack != timeSeries[idx].end()) {
		cv::Rect rcActual = iTrack->getActualEntry().rect();
		cv::Rect rcPrev = iTrack->getPreviousEntry().rect();

		int id = iTrack->getId();
		cv::rectangle(canvas, rcActual, color[id % nColors], Line::thick);
		cv::rectangle(canvas, rcPrev, color[id % nColors], Line::thin);
		++iTrack;
	}

	return;
}


void printTrackInfoAt(cv::Mat& canvas, const TrackTimeSeries& timeSeries, const size_t idxUnchecked) {
	// appearance
	cv::Scalar color[] = {blue, green, red, yellow};
	size_t nColors = sizeof(color) / sizeof(color[0]);	
	int idxLine = 0;


	// check upper index bound
	size_t idx = (idxUnchecked >= timeSeries.size()) ? timeSeries.size()-1 : idxUnchecked;

	std::list<Track>::const_iterator iTrack = timeSeries[idx].begin();
	while (iTrack != timeSeries[idx].end()) {

		// collect track info
		int confidence = iTrack->getConfidence();
		int id = iTrack->getId();
		int length = iTrack->getLength();
		double velocity = iTrack->getVelocity().x;

		// print track info
		std::stringstream ss;
		ss << "#" << id << ", con=" << confidence << ", len=" << length << ", v=" 
			<< std::fixed << std::setprecision(1) << velocity;

		// split status line, if necessary
		int xOffset = 5;
		int font = cv::HersheyFonts::FONT_HERSHEY_SIMPLEX;
		double fontScale = 0.4;
		int baseline = 0;
		cv::Size textSize = cv::getTextSize(ss.str(), font, fontScale, 1, &baseline);

		// fits onto one line 
		if (textSize.width + (xOffset * 2) < canvas.size().width) {
			int yOffset = 10 + idxLine * 10;
			cv::putText(canvas, ss.str(), cv::Point(xOffset, yOffset),
				font, fontScale, color[id % nColors]);

		// needs two lines
		} else {
			size_t pos = std::string::npos;
			pos = ss.str().find(", len=");
			std::string line1 = ss.str().substr(0, pos);
			std::string line2 = ss.str().substr(pos + 2);
			int yOffset = 10 + idxLine * 20;
			cv::putText(canvas, line1, cv::Point(xOffset, yOffset),
				font, fontScale, color[id % nColors]);
			cv::putText(canvas, line2, cv::Point(xOffset, yOffset + 10),
				font, fontScale, color[id % nColors]);
		}
		++idxLine;
		++iTrack;
	} // end_while

	return;
}


bool setRoiSizeToConfig(Config* pConfig, const cv::Size roi) {
	using namespace std;

	// set config param roi
	bool success = pConfig->setParam("roi_width", to_string( static_cast<long long>(roi.width) ));
	success &= pConfig->setParam("roi_height", to_string( static_cast<long long>(roi.height) ));
	if (success) {
		cout << "ROI set to: " << pConfig->getParam("roi_width") << "x" << pConfig->getParam("roi_height") << endl;
	} else {
		cerr << "was not able to set ROI correctly" << endl;
	}

	return success;
}


void showCanvas(const std::string& name, const cv::Mat& canvas, const double scaling = 1.0) { 
		cv::Mat scaledCanvas;
		cv::resize(canvas, scaledCanvas, cv::Size(0,0), scaling, scaling);
		cv::imshow(name, scaledCanvas);
		return;
}





bool showTrackStateAt(const TrackStateVec& state, cv::Size roi, size_t idxUnchecked) {
	using namespace std;
	// check upper index bound
	size_t idx = (idxUnchecked >= state.size()) ? state.size()-1 : idxUnchecked;
	size_t maxIdx = state.size();

	cv::Mat canvas(roi, CV_8UC3, black);
	size_t idxWindow = 0;
	list<TrackState>::const_iterator iState = state[idx].begin();
	while (iState != state[idx].end()) {
		// window appearance
		double scaleX = 2.0;
		int offsetX = 640;
		int winPosX = offsetX + idxWindow * (roi.width * static_cast<int>(scaleX) + 6);
		int menuBar = 30;
		int winHeight = roi.height * static_cast<int>(scaleX) + menuBar;
		// print blobs and tracks on roi-sized canvax
		canvas.setTo(black);
		printBlobs(canvas, iState->m_blobs);
		printTracks(canvas, iState->m_tracks);
		printOcclusions(canvas, iState->m_occlusions);

		// print track info on larger scaled canvas
		cv::Mat canvasLarge;
		cv::resize(canvas, canvasLarge,cv::Size(0,0), 2.0, 2.0);
		printTrackInfo(canvasLarge, iState->m_tracks);
		printIndex(canvasLarge, idx);

		// show window "iState->m_name" 
		// for some weird reasons idx cannot be shown in window title
		cv::namedWindow(iState->m_name);
		cv::moveWindow(iState->m_name, winPosX, 0);
		cv::imshow(iState->m_name, canvasLarge);
		
		++iState;
		++idxWindow;
	}

	return true;
}


bool examineTrackState(const TrackStateVec trackState, cv::Size roi) {
	using namespace std;

	// validity check
	size_t maxIdx = trackState.size();
	if (maxIdx == 0) {
		cerr << "track state does not contain any data" << endl;
		return false;
	} else {
		--maxIdx;
	}

	// print help once
	static bool isInitialized = false;
	if (!isInitialized) {
		cout << endl << "step through TrackState index" << endl;
		cout << "ARROW UP:   step backward" << endl;
		cout << "ARROW DOWN: step forward" << endl;
		cout << "ESC:        exit" << endl << endl;
		isInitialized = true;
	}

	// loop through index
	int key = 0;
	while (key != Key::escape) {
		showTrackStateAt(trackState, roi, g_idx);
		key = cv::waitKeyEx(0);
		switch (key) {
			case Key::arrowUp:
				g_idx = g_idx > 0 ? --g_idx : maxIdx;
				break;
			case Key::arrowDown:
				g_idx = g_idx < maxIdx ? ++g_idx : 0;
				break;
		}
		cout << "idx: " << g_idx << "    \r"; // \r returns to beginning of line
	}
	cout << "end TrackState visualization" << endl;
	return true;
}

namespace vis {
	enum Err { 
		ok		= 0,
		noData = 1,
	};

	int stepThroughBlobs(cv::Mat& canvas, const BlobTimeSeries& timeSeries) {
		using namespace std;
		// set up window
		cv::namedWindow("blobs");
		cv::moveWindow("blobs", 640, 0);

		// check size
		size_t maxIdx = timeSeries.size();
		if (maxIdx == 0)
			return Err::noData;
		else
			--maxIdx;

		// print help
		cout << endl << "blob time series visualization" << endl;
		cout << "ARROW_UP:   decrease index" << endl;
		cout << "ARROW_DOWN: increase index" << endl;
		cout << "ESC:        exit" << endl << endl;

		// loop through time series
		size_t idx = 0;
		int key = 0;
		while (key != Key::escape) {
			canvas.setTo(black);
			printBlobsAt(canvas, timeSeries, idx);
			cv::imshow("blobs", canvas);
			key = cv::waitKeyEx(0);
			//cout << "keycode: " << key << endl;
			switch (key) {
				case Key::arrowUp:
					idx = idx > 0 ? --idx : maxIdx;
					break;
				case Key::arrowDown:
					idx = idx < maxIdx ? ++idx : 0;
					break;
			}
			cout << "idx: " << idx << "    \r"; // \r returns to beginning of line
		}
		cout << "end time series visualization" << endl;
		return Err::ok;
	}

	int stepThroughTracks(cv::Mat& canvas, const TrackTimeSeries& timeSeries) {
		using namespace std;
		// set up window
		cv::namedWindow("tracks");
		cv::moveWindow("tracks", 1052, 0);

		// check size
		size_t maxIdx = timeSeries.size();
		if (maxIdx == 0)
			return Err::noData;
		else
			--maxIdx;

		// print help
		cout << endl << "track time series visualization" << endl;
		cout << "ARROW_UP:   decrease index" << endl;
		cout << "ARROW_DOWN: increase index" << endl;
		cout << "ESC:        exit" << endl << endl;

		// loop through time series
		size_t idx = 0;
		int key = 0;
		while (key != Key::escape) {
			canvas.setTo(black);
			printTracksAt(canvas, timeSeries, idx);
			cv::imshow("tracks", canvas);
			key = cv::waitKeyEx(0);
			//cout << "keycode: " << key << endl;
			switch (key) {
				case Key::arrowUp:
					idx = idx > 0 ? --idx : maxIdx;
					break;
				case Key::arrowDown:
					idx = idx < maxIdx ? ++idx : 0;
					break;
			}
			cout << "idx: " << idx << "    \r"; // \r returns to beginning of line
		}
		cout << "end time series visualization" << endl;
		return Err::ok;
	}

} // end namespace vis


bool testOcclusion(std::list<Occlusion>& oList, IdGen* pIdGen, cv::Size roi, Track* movingRight, Track* movingLeft) {
	Occlusion occ(roi, movingRight, movingLeft, 5);
	oList.push_back(occ);
	return true;
}


int main(int argc, char* argv[]) {
	using namespace std;

	// visual debugging of assignment process in occlusion
	// create two blobs moving in opposite direction
	IdGen occIDs(6);
	cv::Size roi(100, 100);
	cv::Size sizeBlob(30,20);

	// set up scene
	Config config;
	Config* pConfig = &config;
    setRoiSizeToConfig(pConfig, roi);
	SceneTracker scene(pConfig); 
	SceneTracker* pScene = &scene;
	config.attach(pScene);

	// test occlusion c'tor and d'tor
	/*
	Track right(1);
	Track left(2);

	OcclusionIdList occIdList(3);
	Occlusion occ(roi, &right, &left, 10);
	cout << "occ id: " << occ.id() << endl;
	waitForEnter();

	occIdList.add(occ);
	const list<Occlusion>* pOccList = occIdList.getList();
	cout << "occ id in list: " << pOccList->front().id() << endl;
	waitForEnter();
	*/

	// create blobs for occlusion
	BlobTimeSeries blobTmSer = createOcclusion(roi, sizeBlob);
	TrackTimeSeries trackTmSer;
	cv::Mat canvas(roi, CV_8UC3, black);

	// loop through blobList
	for (g_idx = 0; g_idx < blobTmSer.size(); ++g_idx) {
		cout << endl << "new idx: " << g_idx << endl;

		// copy blob list of current time step to avoid deletion
		std::list<cv::Rect> currentBlobs(blobTmSer[g_idx]);
		std::list<Track>* pTracks =  scene.updateTracksIntersect(currentBlobs, g_idx);
		std::list<Track> tracks(*pTracks); // debugging only
		trackTmSer.push_back(tracks); // release: push_back(*pTracks)

		// DEBUG
		/*
		canvas.setTo(black);
		printBlobsAt(canvas, blobTmSer, n);
		showCanvas("blobs", canvas, 2);
		*/
		/*
		canvas.setTo(black);
		printTracksAt(canvas, trackTmSer, g_idx);
		Occlusions* pOccs(scene.getOcclusions());
		printOcclusions(canvas, pOccs);

		cv::Mat canvasLarge;
		cv::resize(canvas, canvasLarge,cv::Size(0,0), 2.0, 2.0);
		printTrackInfoAt(canvasLarge, trackTmSer, g_idx);
		showCanvas("tracks", canvasLarge, 1);
		
		
		showTrackStateAt(g_trackState, roi, g_idx);
		if (breakEscContinueEnter())
			break;
		*/
	}

	examineTrackState(g_trackState, roi);
	//showTrackStateAt(g_trackState, roi, 20);
	//breakEscContinueEnter();

//	showTrackStateAt(g_trackState, roi, n);
	
	//Occlusion occ(&occIDs, roi, &trackRight, &trackLeft, 5); 
	// cout << "Occlusion 1 ID:" << occ.id() << endl;
	// occ.assignBlobs

	// int error = vis::stepThroughBlobs(canvas, blobTmSer);
	// if (error == vis::Err::noData) 	cout << "no data in time series" << endl;

	//waitForEnter();
	return 0;
}
