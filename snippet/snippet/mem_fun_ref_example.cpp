void Track::checkPosAndDir() {
	//int countPos = mFramesize / 2;
	int countPos = 90; // roi.width/2
	double trackLength;
	if (this->getConfidence() > 4) {
		if (signBit(this->getVelocity().x)) { // moving to left
			if (this->getActualEntry().centroid().x < countPos) {
				mCounted = true;
			}
		}
		else {// moving to right
			if (this->getActualEntry().centroid().x > countPos)
				mCounted = true;
		}
	}
	cout << "check pos and dir" << endl;
}

/// count vehicles: count only tracks above defined confidence level
void SceneTracker::countCars() {
	// track confidence > x
	// moving left && position < yy
	// moving right && position > yy
	for_each(mTracks.begin(), mTracks.end(), mem_fun_ref(&Track::checkPosAndDir));
}