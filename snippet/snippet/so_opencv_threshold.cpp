#include "stdafx.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/config.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/frame_handler.h"


/// pause console until <enter> has been pressed
bool waitForEnter();




int main(int argc, char* argv[]) {
	using namespace std;

	string file("D:/Holger/app-dev/cpp/snippet/snippet/opencv-test/FSUo3.png");
	if (!isFileExist(file)) {
		cerr << "file path not valid: " << file << endl;
	} else {

		cv::Mat img = cv::imread(file);
		cv::Mat img_grey;
		cv::cvtColor(img, img_grey, cv::COLOR_BGR2GRAY);
		cv::imshow("src", img);


		cv::Mat img_thresh;
		cv::threshold(img, img_thresh, 110, UCHAR_MAX, cv::THRESH_BINARY);
		cv::imshow("threshold", img_thresh);

		/*
		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(img_thresh, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
			
				cv::Mat frame(img.size(), CV_8UC3);
		for (int i = 0; i < hierarchy.size(); ++i) 
			cv::drawContours(frame, contours, i, red);
			*/


	}

	while (cv::waitKey(10) != 27) {};
	cout << "ESC pressed -> end video processing" << endl;
	
	//waitForEnter();
	return 0;
}

	






