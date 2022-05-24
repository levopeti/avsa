#ifndef HISTOGRAM_HPP_
#define HISTOGRAM_HPP_

#include "opencv2/opencv.hpp"

typedef enum {
	GRAY_LEVEL=0,
	HSV_H=1,
	HSV_S=2,
	RGB_R=3,
	RGB_G=4,
    RGB_B=5,
    GRADIENT=6
} CLASS;

static const char *enum_str[] = {
	 "GRAY SCALE HISTOGRAM",
	 "HSV: H HISTOGRAM",
	 "HSV: S HISTOGRAM",
	 "RGB: R HISTOGRAM",
	 "RGB: G HISTOGRAM",
	 "RGB: B HISTOGRAM",
	 "GRADIENT HISTOGRAM"};


// Compute histogram
cv::Mat create_histogram(cv::Mat Frame, cv::Rect Bb, int bins, CLASS feature, cv::Size s_block, cv::Size s_cell, cv::Size s_stride, bool resize_img);

// Compute gradients
cv::Mat computeHOGs(cv::Mat gray, int bins, cv::Size s_block, cv::Size s_cell, cv::Size s_stride, bool resize_img);


#endif