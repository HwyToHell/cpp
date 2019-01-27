#include <opencv2/opencv.hpp>

#include "vibe-background-sequential.h"

/// background subtractor: first order low pass filter
class BackgroundSubtractorLowPass : public cv::BackgroundSubtractor {
public:
	BackgroundSubtractorLowPass(double alpha, double threshold);
	~BackgroundSubtractorLowPass();
	virtual void apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate=-1);
	virtual void getBackgroundImage(cv::OutputArray backgroundImage) const;
private:
	cv::Mat	m_accu;
	double	m_alpha;
	bool	m_isInitialized;
	double	m_threshold;
};

cv::Ptr<BackgroundSubtractorLowPass> createBackgroundSubtractorLowPass(double alpha, double threshold);


/// background subtractor: ViBe (visual background extractor)
class BackgroundSubtractorVibe :  public cv::BackgroundSubtractor {
public:
	BackgroundSubtractorVibe();
	~BackgroundSubtractorVibe();
	virtual void apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate=-1);
	virtual void getBackgroundImage(cv::OutputArray backgroundImage) const;
private:
	vibeModel_Sequential_t*		m_model;
	bool						m_isInitialized;
};
