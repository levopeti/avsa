/*
 * CandMatching.h
 *
 *  Created on: May 14, 2022
 *      Author: levcsi
 */

#ifndef SRC_CANDMATCHING_HPP_
#define SRC_CANDMATCHING_HPP_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include "histogram.hpp"

using namespace std;
using namespace cv;

class CandMatching {
	Rect _model;
	vector<Rect> _candidateRects;
	vector<float> _matchScores;
public:
	CandMatching(Rect model);
	virtual ~CandMatching();

	void makeCandGrid(Mat &frame, vector<int> step_nums, vector<int> step_sizes);
	void makeMatchScores(Mat frame, Mat his_gt, int bins, CLASS feature, cv::Size s_block, cv::Size s_cell, cv::Size s_stride, bool resize_img);
	Rect candWithMinScore();
	void drawAllCand(Mat &frame);
};


#endif /* SRC_CANDMATCHING_HPP_ */
