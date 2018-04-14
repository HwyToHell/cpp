#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;


struct TrackEntry {
	cv::Point2i mCentroid;
	TrackEntry(int _x, int _y): mCentroid(_x, _y) {};
	cv::Point2i centroid() {
		return mCentroid;
	};
};


cv::Point2d averageVelocity(vector<TrackEntry> mHistory, cv::Point2d& mAvgVelocity) {
	const int window = 3;
	int lengthHistory = mHistory.size();

	if (lengthHistory > 1) {
		int idxMax = lengthHistory - 1;
		cv::Point2d actVelocity = mHistory[idxMax].centroid() - mHistory[idxMax-1].centroid();

		if (idxMax <= window) {// window not fully populated yet
			mAvgVelocity = (mAvgVelocity * (double)(idxMax - 1) + actVelocity) * (1.0 / (double)(idxMax));
		}
		else { // window fully populated
			cv::Point2d oldVelocity = mHistory[idxMax-window].centroid() - mHistory[idxMax-window-1].centroid();
			mAvgVelocity += (actVelocity - oldVelocity) * (1.0 / (double)window);
		}
	}
	return mAvgVelocity;
}

int main(int argc, char* argv[]) {

	vector<TrackEntry> history;
	cv::Point2d avgVelocity(0, 0);

	int x = 0;
	double intDx = 0;
	double intVel = 0;
	double dx = 0;

	for (int i = 0; i < 9; ++i) {
		x += (10 + i);
		cout << endl << "pass: " << i << "  x: " << setw(3) << x;
		history.push_back(TrackEntry(x, 0));
		if (i > 0) {
			cv::Point2d dPoint = history[i].centroid() - history[i-1].centroid();
			dx = dPoint.x; 
			cout << fixed << setprecision(2);
			cout << " dx: " << setw(4) << dx;
			intDx += dx;
			intVel = intDx / i;
			cout << endl << "integr avgVelocity: " << fixed << setprecision(2) << intVel;
			double velocity = averageVelocity(history, avgVelocity).x;
			cout << endl << "       avgVelocity: " << setprecision(2) << velocity << endl;
		}

	}


	cout << endl << "Press <enter> to continue" << endl;
	string str;
	getline(cin, str);

	return 0;
}


/*void Track::UpdateAvgVelocity()
{
	const int window = 5;
	int lengthHistory = history.size();
	cv::Point2d avg = cv::Point2d(0,0);

	// iterative formula
	if (lengthHistory > window) // moving average over n-window values
	{
		for (int i = lengthHistory - (int)window; i < lengthHistory; ++i)
			avg += (cv::Point2d)history[i].velocity;
		avg.x /= window;
		avg.y /= window;
	}
	else // if window not fully populated, take actual value
	{
		if (lengthHistory > 1) 
		{
			avg = history[lengthHistory-1].velocity;
		}
	}
	
	
	// recursive formula
	int n = lengthHistory - 1; 	
	if (n > 0) // more than 1 TrackEntries needed in order to calculate velocity
	{
		if (n < window+1) // window not fully populated yet
		{
			avgVelocity.x = (avgVelocity.x * (n - 1) + history[n].velocity.x) / n;
			avgVelocity.y = (avgVelocity.y * (n - 1) + history[n].velocity.y) / n;
		}
		else // moving average over <window> values
		{
			avgVelocity.x += ((history[n].velocity.x - history[n-(int)window].velocity.x) / (double)window);
			avgVelocity.y += ((history[n].velocity.y - history[n-(int)window].velocity.y) / (double)window);
		}
	}
	return;
}*/