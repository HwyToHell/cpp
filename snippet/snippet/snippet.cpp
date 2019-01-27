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


bool addOcclusion(std::list<Occlusion>& lst) {
	cv::Size roi(100,100);
	Track trackRight(1);
	Track trackLeft(2);
	Occlusion occ(roi, &trackRight, &trackLeft, 1);
	lst.push_back(occ);
	return true;
}


int main(int argc, char* argv[]) {
	using namespace std;

	list<Occlusion> l;
	addOcclusion(l);
	cout << "add occlusion 1: " << " id: " << l.back().id() << endl;
	cout << endl;

	// copy construction is allowed
	addOcclusion(l);
	cout << "add occlusion 2: " << " id: " << l.back().id() << endl;
	cout << endl;

	l.clear();
	cout << "list cleared" << endl << endl;

	waitForEnter();
	return 0;
}

	






