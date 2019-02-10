#include "stdafx.h"

#include "../../cpp/inc/id_pool.h"
#include "../../cpp/inc/pick_list.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/tracker.h"


/// pause console until <enter> has been pressed
bool waitForEnter();


void createOcclusion(IdGen* pIds, cv::Size roi, Track* trRight, Track* trLeft) {
	using namespace std;
	int steps = 10;
	Occlusion occ1(pIds, roi, trRight, trLeft, steps); 
	cout << "Occlusion 1 ID:" << occ1.id() << endl;

	Occlusion occ2(pIds, roi, trRight, trLeft, steps); 
	cout << "Occlusion 2 ID:" << occ2.id() << endl;
}



int main(int argc, char* argv[]) {
	using namespace std;

	IdGen occIDs(3);
	cv::Size roi(100, 100);
	Track trackRight(1);
	Track trackLeft(2);

	createOcclusion(&occIDs, roi, &trackRight, &trackLeft);

	Occlusion occ(&occIDs, roi, &trackRight, &trackLeft, 5); 
	cout << "Occlusion 1 ID:" << occ.id() << endl;

	waitForEnter();
	return 0;
}

	






