void Track::UpdateAvgVelocity()
{
	const int window = 5;
	int lengthHistory = history.size();
	cv::Point2d avg = cv::Point2d(0,0);

	/*	// iterative formula
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
	*/
	
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
}