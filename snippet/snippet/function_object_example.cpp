#include "stdafx.h"
#include <climits>
#include <functional>
#pragma warning(disable: 4482) // MSVC10: enum nonstd extension

#include "D:/Holger/app-dev/video-dev/car-count/include/config.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/frame_handler.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/tracker.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/recorder.h"
//#include "../../inc/program_options.h"
#include "../../inc/opencv/backgroundsubtraction.h"


/// pause console until <enter> has been pressed
bool waitForEnter();


int main(int argc, char* argv[]) {
	using namespace std;


	// find tracks by ID
	std::list<Track> tracks;
	cv::Size size(30,20);
	cv::Point origin(0,0);
	cv::Size roi(100,100);

	for(int i=1; i<=4; ++i) {
		Track track(i);
		origin += cv::Point(0,20);
		cv::Rect rect(origin, size);
		track.addTrackEntry(rect, roi);
		tracks.push_back(track);
		cout << "id: " << i << " rect: " << rect << endl;
	}

	list<Track>::iterator it;
	it = find_if(tracks.begin(), tracks.end(), TrackID_eq(2));
	if (it != tracks.end()) {
		cout << "found ID: " << it->getId() << endl;
	} else {
		cout << "ID not found" << endl;
	}



	waitForEnter();
	return 0;
}