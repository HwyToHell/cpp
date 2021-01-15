#include "stdafx.h"

#include "../../inc/opencv/backgroundsubtraction.h"


/// background subtractor: first order low pass filter
cv::Ptr<BackgroundSubtractorLowPass> createBackgroundSubtractorLowPass(double alpha, double threshold) {
    return cv::makePtr<BackgroundSubtractorLowPass>(alpha, threshold);
}

BackgroundSubtractorLowPass::BackgroundSubtractorLowPass(double alpha, double threshold) : 
	m_alpha(alpha), 
	m_isInitialized(false),
	m_threshold(threshold) {
}

BackgroundSubtractorLowPass::~BackgroundSubtractorLowPass() {
}

void BackgroundSubtractorLowPass::apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate) {
    std::ignore = learningRate;
	// fill accu when applying for first time
	if (!m_isInitialized) {
		m_accu = cv::Mat(image.size(), CV_32F);
		image.getMat().convertTo(m_accu, CV_32F);
		fgmask.assign(cv::Mat(image.size(), CV_8UC1));
		m_isInitialized = true;
	// actual segmentation algorithm
	} else { 
		cv::accumulateWeighted(image, m_accu, m_alpha);
		cv::Mat accu8U;
		cv::convertScaleAbs(m_accu, accu8U);
		cv::absdiff(image, accu8U, fgmask);
		cv::cvtColor(fgmask, fgmask, cv::COLOR_BGR2GRAY);
		cv::threshold(fgmask, fgmask, m_threshold, UCHAR_MAX, CV_THRESH_BINARY);
	}

	return;
}

void BackgroundSubtractorLowPass::getBackgroundImage(cv::OutputArray backgroundImage) const {
	m_accu.convertTo(backgroundImage, CV_8U);
	return;
}


/// background subtractor: ViBE (Visual Background Extractor)
/// works with both RGB and GRAY images
BackgroundSubtractorVibe::BackgroundSubtractorVibe() :
	m_isInitialized(false),
	m_model(nullptr)
	{}

BackgroundSubtractorVibe::~BackgroundSubtractorVibe() {
	int ret = libvibeModel_Sequential_Free(m_model);
	ret = 0;
    (void)ret;
}

void BackgroundSubtractorVibe::apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate) {
    std::ignore = learningRate;

	cv::Mat input;
	cv::cvtColor(image.getMat(), input, cv::COLOR_BGR2HSV);
	//cv::cvtColor(image.getMat(), input, cv::COLOR_BGR2GRAY);

	// initialize model at first frame
	if (!m_isInitialized) {
		m_model = libvibeModel_Sequential_New();
		fgmask.assign(cv::Mat(input.size(), CV_8UC1));

		// RGB image
		if (input.channels() == 3) {
            libvibeModel_Sequential_AllocInit_8u_C3R(m_model, input.data,
                                                     static_cast<uint32_t>(input.cols),
                                                     static_cast<uint32_t>(input.rows));
		// grayscale image
		} else {
            libvibeModel_Sequential_AllocInit_8u_C1R(m_model, input.data,
                                                     static_cast<uint32_t>(input.cols),
                                                     static_cast<uint32_t>(input.rows));
		}

		m_isInitialized = true;
	}

	// continous update after initialization

	// necessary to properly allocate array, to be called before OutputArray::getMat()
	fgmask.create(input.size(), CV_8UC1); 

	// RGB image
	if (input.channels() == 3) {
		libvibeModel_Sequential_Segmentation_8u_C3R(m_model, input.data, fgmask.getMat().data);
		libvibeModel_Sequential_Update_8u_C3R(m_model, input.data, fgmask.getMat().data);
	// grayscale image
	} else {
		libvibeModel_Sequential_Segmentation_8u_C1R(m_model, input.data, fgmask.getMat().data);
		libvibeModel_Sequential_Update_8u_C1R(m_model, input.data, fgmask.getMat().data);
	}

}

void BackgroundSubtractorVibe::getBackgroundImage(cv::OutputArray backgroundImage) const {
    std::ignore = backgroundImage;
}
