/*
 * CandMatching.cpp
 *
 *  Created on: May 14, 2022
 *      Author: levcsi
 */

#include "CandMatching.hpp"
#include "histogram.hpp"

CandMatching::CandMatching(Rect model) {
	_model = model;
}

CandMatching::~CandMatching() {
}

void CandMatching::makeCandGrid(Mat &frame, vector<int> step_nums, vector<int> step_sizes){
	for(int i = -step_nums[0]; i <= step_nums[0]; i++){
		for(int j = -step_nums[1]; j <= step_nums[1]; j++){
			int x = std::min(std::max(_model.x + i * step_sizes[0], 0), frame.cols - _model.width);
			int y = std::min(std::max(_model.y + j * step_sizes[1], 0), frame.rows - _model.height);

			Rect candidateRect(x, y, _model.width, _model.height);
			_candidateRects.push_back(candidateRect);
		}
	}
}

void CandMatching::makeMatchScores(Mat frame, Mat his_gt, int bins, CLASS feature, cv::Size s_block, cv::Size s_cell, cv::Size s_stride, bool resize_img){
	// cvtColor(frame, frame, cv::COLOR_RGB2GRAY);
	// frame.convertTo(frame, CV_32FC1);
	// Mat model_matrix = frame(_model);

	// Histogram and score
	Mat hist_frame;
	double score_hist;

	for(size_t i=0; i < _candidateRects.size(); i++){
		Mat current_candidate = frame(_candidateRects[i]);

		// int result_cols = model_matrix.cols - current_candidate.cols + 1;
		// int result_rows = model_matrix.rows - current_candidate.rows + 1;

		// Mat result;
		// result.create(result_rows, result_cols, CV_32FC1);


		hist_frame = create_histogram(frame, _candidateRects[i], bins, feature, s_block,  s_cell,  s_stride, resize_img);
		score_hist = compareHist(his_gt, hist_frame, CV_COMP_BHATTACHARYYA);

		// matchTemplate(current_candidate, model_matrix, result, CV_TM_CCORR);
		_matchScores.push_back(score_hist);
	
}
}


Rect CandMatching::candWithMinScore(){
	float min = numeric_limits<float>::max();
	int arg = 0;

	for(size_t i=0; i<_candidateRects.size(); i++){
		if(_matchScores[i] < min){
			min = _matchScores[i];
			arg = i;
		}
	}
	return _candidateRects[arg];
}

void CandMatching::drawAllCand(Mat &frame){
	for(size_t i=0; i<_candidateRects.size(); i++)
		rectangle(frame, _candidateRects[i], Scalar(255,0,255));
}

