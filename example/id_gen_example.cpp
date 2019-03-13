#include "stdafx.h"

#include "D:/Holger/app-dev/cpp/inc/id_pool.h"
#include "D:/Holger/app-dev/cpp/inc/pick_list.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/config.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/tracker.h"

/// pause console until <enter> has been pressed
bool waitForEnter();
bool setRoiToConfig(Config* pConfig, const cv::Size roi);
bool testOcclusion(std::list<Occlusion>& oList, IdGen* pIdGen, cv::Size roi, Track* movingRight, Track* movingLeft);


int main(int argc, char* argv[]) {
	using namespace std;
	
	/// use case scene tracker

	// ID Gen instance with 9 uint
	IdGen occIds(9);
	cv::Size roi(100, 100);
	cout << "pool size: " << occIds.poolSize() << endl;
	
	// set up scene
	Config config;
	Config* pConfig = &config;
	setRoiToConfig(pConfig, roi);
	SceneTracker scene(pConfig); 
	SceneTracker* pScene = &scene;
	config.attach(pScene);

	// test occlusion c'tor and d'tor
	Track right(1);
	Track left(2);	

	// construct and destroy occlusion
	list<Occlusion> occList;
	testOcclusion(occList, &occIds, roi, &right, &left);
	cout << "list entry copy constructed" << endl;
	waitForEnter();

	Occlusion* occ2 = new Occlusion(&occIds, roi, &right, &left, 20);
	cout << "occ2 after creation: " << occ2->id() << endl;
	cout << "list before assignment: " << occList.begin()->id() << endl;
	*occ2 = occList.back();

	// assignment operator swaps ids
	cout << "occ2 after assignment: " << occ2->id() << endl;
	cout << "list after assignment: " << occList.begin()->id() << endl;
	cout << "list entry assigned to heap object occ2" << endl;
	waitForEnter();

	list<Occlusion>::const_iterator it = occList.begin();
	occList.erase(it);
	cout << "list entry erased" << endl;
	waitForEnter();

	delete occ2;
	cout << "heap object occ2 destoyed" << endl;
	waitForEnter();
	return 0;
}