#ifndef KALMAN_FILTER_H_INCLUDE
#define KALMAN_FILTER_H_INCLUDE

#include "opencv2/opencv.hpp"



// Create the kalman filters
cv::KalmanFilter create_kalman_filter(int stateSize);

// Compute the estimations
void do_kalman(cv::Point meas, int stateSize, cv::KalmanFilter &kf, bool &first_detection, std::vector<cv::Point> &v_estimations, std::vector<cv::Point> &v_measurements);

// Draw the final trajectory
void draw_trajectory(std::vector<cv::Point> v, cv::String legend, cv::Mat &frame, CvScalar color, cv::Point origin_point);






















#endif