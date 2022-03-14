/* Applied Video Sequence Analysis (AVSA)
 *
 *	LAB1.0: Background Subtraction - Unix version
 *	fgesg.cpp
 *
 * 	Authors: José M. Martínez (josem.martinez@uam.es), Paula Moral (paula.moral@uam.es) & Juan Carlos San Miguel (juancarlos.sanmiguel@uam.es)
 *	VPULab-UAM 2020
 */

#include <opencv2/opencv.hpp>
#include "fgseg.hpp"
#include <stdlib.h>
#include <algorithm>
#include <cmath>

using namespace fgseg;


//default constructor
bgs::bgs(double threshold, double alpha, bool selective_bkg_update, int threshold_ghosts2,
		bool rgb, double alpha_sh, double beta_sh, double saturation_th, double hue_th,
		double sigma_coef, bool unimodal, int K, double initial_variance, double W_th)
{
	_threshold=threshold;
	_alpha=alpha;
	_rgb=rgb;
	_selective_bkg_update=selective_bkg_update;
	_threshold_ghosts2=threshold_ghosts2;
	_alpha_sh = alpha_sh;
	_beta_sh = beta_sh;
	_saturation_th = saturation_th;
	_hue_th = hue_th;
	_sigma_coef = sigma_coef;
	_unimodal = unimodal;
	_K = K;
	_initial_variance = initial_variance;
	_W_th = W_th;
}

//default destructor
bgs::~bgs(void)
{
}

//method to initialize bkg (first frame - hot start)
void bgs::init_bkg(cv::Mat Frame)
{
	if(!_rgb)
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color

	_bkg = Frame.clone();
	_fgcounter = Mat::zeros(Size(Frame.cols, Frame.rows), CV_16UC1);

	// DOUBLES
	_sum = Mat::zeros(Size(Frame.cols, Frame.rows), CV_64F);
	_sum_squares = Mat::zeros(Size(Frame.cols, Frame.rows), CV_64F);

	if(!_unimodal){ //CV_32FC(nbChannels)
		for(int k=0; k< _K; k++){
			_mean_mm.push_back(Mat::zeros(Size(Frame.cols, Frame.rows), CV_64F));
			_variance_mm.push_back(Mat::zeros(Size(Frame.cols, Frame.rows), CV_64F));
			_omega_mm.push_back(Mat::zeros(Size(Frame.cols, Frame.rows), CV_64F));
		}
	}
}

//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{
	if(!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		Frame.copyTo(_frame);

		absdiff(_bkg, Frame, _diff);
		threshold(_diff, _bgsmask, _threshold, 255, cv::THRESH_BINARY);

		if (_selective_bkg_update)
		{
			for(int i=0; i<_bgsmask.rows; ++i)
				for(int j=0; j<_bgsmask.cols; ++j)
				{
					if (!_bgsmask.at<uchar>(i, j))
						_bkg.at<uchar>(i, j) = _alpha * Frame.at<uchar>(i, j) + (1 - _alpha) * _bkg.at<uchar>(i, j);
				}
		}
		else
			_bkg = _alpha * Frame + (1 - _alpha) * _bkg;

		for(int j=0; j<_bgsmask.rows; ++j)
			for(int i=0; i<_bgsmask.cols; ++i)
			{
				_fgcounter.at<ushort>(j, i) += _bgsmask.at<bool>(j, i) / 255;
				_fgcounter.at<ushort>(j, i) *= _bgsmask.at<bool>(j, i) / 255;
				if (_fgcounter.at<ushort>(j, i) >= _threshold_ghosts2)
				{
					_bkg.at<uchar>(j, i) = Frame.at<uchar>(j, i);
					_fgcounter.at<ushort>(j, i) = 0;
				}
			}
	}
	else{
		Frame.copyTo(_frame);
		_bgsmask = Mat::zeros(Size(Frame.cols, Frame.rows), CV_8UC1);
		cv::Mat bgsmask_rgb;
		cv::Mat rgbimg[3];

		absdiff(_bkg, Frame, _diff);
		threshold(_diff, bgsmask_rgb, _threshold, 255, cv::THRESH_BINARY);

		split(bgsmask_rgb, rgbimg);
		_bgsmask = rgbimg[0] + rgbimg[1] + rgbimg[2];

		if(_selective_bkg_update)
		{
			for(int i=0; i<_bgsmask.rows; ++i)
				for(int j=0; j<_bgsmask.cols; ++j)
				{
					if (!_bgsmask.at<uchar>(i, j))
						_bkg.at<Vec3b>(i, j) = _alpha * Frame.at<Vec3b>(i, j) + (1 - _alpha) * _bkg.at<Vec3b>(i, j);
				}
		}

		else
			_bkg = _alpha * Frame + (1 - _alpha) * _bkg;

		for(int j=0; j<_bgsmask.rows; ++j)
			for(int i=0; i<_bgsmask.cols; ++i)
			{
				_fgcounter.at<ushort>(j, i) += _bgsmask.at<bool>(j, i) / 255;
				_fgcounter.at<ushort>(j, i) *= _bgsmask.at<bool>(j, i) / 255;
				if (_fgcounter.at<ushort>(j, i) >= _threshold_ghosts2)
				{
					_bkg.at<Vec3b>(j, i) = Frame.at<Vec3b>(j, i);
					_fgcounter.at<ushort>(j, i) = 0;
				}
			}
	    }
}

//method to detect and remove shadows in the BGS mask to create FG mask
void bgs::removeShadows()
{
	if(!_rgb){
		absdiff(_bgsmask, _bgsmask, _shadowmask);// currently void function mask=0 (should create shadow mask)
	}
	else{
		// init Shadow Mask (currently Shadow Detection not implemented)
		_shadowmask = Mat::zeros(Size(_bgsmask.cols, _bgsmask.rows), CV_8UC1);

		cv::Mat _frame_HSV;
		cv::Mat _bkg_HSV;
		cvtColor(_frame, _frame_HSV, cv::COLOR_BGR2HSV);
		cvtColor(_bkg, _bkg_HSV, cv::COLOR_BGR2HSV);


		for(int j=0; j<_bkg.rows; ++j)
			for(int i=0; i<_bkg.cols; ++i)
			{
				double IH = _frame_HSV.at<cv::Vec3b>(j, i)[0];
				double IS = _frame_HSV.at<cv::Vec3b>(j, i)[1];
				double IV = _frame_HSV.at<cv::Vec3b>(j, i)[2];
				double BH = _bkg_HSV.at<cv::Vec3b>(j, i)[0];
				double BS = _bkg_HSV.at<cv::Vec3b>(j, i)[1];
				double BV = _bkg_HSV.at<cv::Vec3b>(j, i)[2];

				double Dh = min(abs(IH - BH), 360 - abs(IH - BH));

				if ((IV / BV) >= _alpha_sh && (IV / BV) <= _beta_sh && abs(IS - BS) <= _saturation_th && Dh <= _hue_th){
					_shadowmask.at<uchar>(j, i) = 255;
				}
			}

		// Shadow mask set to 1 only when both the shadow and the background are 1 (there are other options)
		for(int i=0; i<_bgsmask.rows; i++){
			for(int j=0; j<_bgsmask.cols; j++)
			{
				_shadowmask.at<uchar>(i, j) = _shadowmask.at<uchar>(i, j) * int(_bgsmask.at<uchar>(i, j));
			}
		}
	}

	absdiff(_bgsmask, _shadowmask, _fgmask); // eliminates shadows from bgsmask
}

void bgs::updateGaussian(cv::Mat Frame, int frame_idx)
{
	// https://math.stackexchange.com/questions/2148877/iterative-calculation-of-mean-and-standard-deviation
	if(!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		Frame.copyTo(_frame);
		Frame.convertTo(Frame, CV_64F);
		_bgsmask = Mat::zeros(Size(Frame.cols, Frame.rows), CV_8UC1);
		_diff = Mat::zeros(Size(Frame.cols, Frame.rows), CV_8UC1);

		if (_unimodal){
			// Set initial values with the 10 first frames
			if (frame_idx <= 10){
				_sum += Frame;
				_sum_squares +=  Frame.mul(Frame);
				_mean = _sum / frame_idx;
				_variance = (_sum_squares / frame_idx) - (_mean.mul(_mean));
			}
			else{
				Mat diff;
				absdiff(Frame, _mean, diff);

				for(int i=0; i<_bgsmask.rows; i++)
					for(int j=0; j<_bgsmask.cols; j++)
						if(diff.at<double>(i,j) > _sigma_coef * sqrt(_variance.at<double>(i,j))){
							_bgsmask.at<uchar>(i,j) = 255;
//							_mean.at<double>(i,j) = _alpha * Frame.at<double>(i,j) + (1 - _alpha) * _mean.at<double>(i,j);
//							_variance.at<double>(i,j) = _alpha * pow(Frame.at<double>(i,j) - _mean.at<double>(i,j), 2)+ (1 - _alpha) * _variance.at<double>(i,j);
						}
						else{
							_bgsmask.at<uchar>(i,j) = 0;
							_mean.at<double>(i,j) = _alpha * Frame.at<double>(i,j) + (1 - _alpha) * _mean.at<double>(i,j);
							_variance.at<double>(i,j) = _alpha * pow(Frame.at<double>(i,j) - _mean.at<double>(i,j), 2)+ (1 - _alpha) * _variance.at<double>(i,j);
						}

				// from double to uchar so it can be displayed
				diff.convertTo(_diff, CV_8UC1);
				}
		}
		else{
			if (frame_idx == 1){
				for(int i=0; i<_bgsmask.rows; i++)
					for(int j=0; j<_bgsmask.cols; j++){
						_mean_mm[0].at<double>(i,j) = Frame.at<double>(i,j);
						_variance_mm[0].at<double>(i,j) = _initial_variance;
						_omega_mm[0].at<double>(i,j) = 1.;
						}
			}


			// from here it is just a copy
			cv::Mat M_tmp = Mat::zeros(Size(Frame.cols, Frame.rows), CV_8UC(_K));
			cv::Mat M[_K];
			split(M_tmp, M);

			cv::Mat omega_mm_tmp_tmp = Mat::zeros(Size(Frame.cols, Frame.rows), CV_64FC(_K));
			cv::Mat omega_mm_tmp[_K];
			split(omega_mm_tmp_tmp, omega_mm_tmp);

			cv::Mat diff_tmp = Mat::zeros(Size(Frame.cols, Frame.rows), CV_64FC(_K));
			cv::Mat diff[_K];


			for(int k=0; k<_K; k++){
				absdiff(Frame, _mean_mm[k], diff[k]);

				for(int i=0; i<_bgsmask.rows; i++)
					for(int j=0; j<_bgsmask.cols; j++){
						if(diff[k].at<double>(i, j) <= _sigma_coef * sqrt(_variance_mm[k].at<double>(i, j))){
							M[k].at<uchar>(i, j) = 1;
							_mean_mm[k].at<double>(i, j) = _alpha * Frame.at<double>(i,j) + (1 - _alpha) * _mean_mm[k].at<double>(i, j);
							_variance_mm[k].at<double>(i, j) = _alpha * pow(Frame.at<double>(i, j) - _mean_mm[k].at<double>(i, j), 2)+ (1 - _alpha) * _variance_mm[k].at<double>(i, j);
						}
						omega_mm_tmp[k].at<double>(i,j) = (1 - _alpha) * _omega_mm[k].at<double>(i,j) + _alpha * int(M[k].at<uchar>(i,j));
					}}

			for(int i=0; i<_bgsmask.rows; i++)
				for(int j=0; j<_bgsmask.cols; j++){
					double sum = 0;

					for(int k=0; k<_K; k++){
						sum += omega_mm_tmp[k].at<double>(i, j);
					}

					for(int k=0; k<_K; k++){
						omega_mm_tmp[k].at<double>(i, j) /= sum;
					}
				}

			for(int i=0; i<_bgsmask.rows; i++){
				for(int j=0; j<_bgsmask.cols; j++){
					int min_arg = -1;
					double min_value = 1;

					for(int k=0; k<_K; k++){
						if(int(M[k].at<uchar>(i, j)) && omega_mm_tmp[k].at<double>(i, j) >= (1 - _W_th)){
							_bgsmask.at<uchar>(i, j) = 0;
							break;
						}
						else{
							_bgsmask.at<uchar>(i, j) = 255;
						}
						if(omega_mm_tmp[k].at<double>(i, j) < min_value){
							min_value = omega_mm_tmp[k].at<double>(i, j);
							min_arg = k;
						}
					}

					if(_bgsmask.at<uchar>(i, j) == 255){

						int sum_match = 0;
						if(_selective_bkg_update)
							sum_match = 1;
						else{// BLIND MODE
							// Check if the pixel belongs to any of the gaussians being foreground
							for(int k=0; k<_K; k++){
								if(int(M[k].at<uchar>(i,j))==1)
									sum_match+=1;}
						}
						// Remove and create new gaussian if the pixel doesn't belong to any
						if(!sum_match){
							omega_mm_tmp[min_arg].at<double>(i, j) = 0.05;
							_mean_mm[min_arg].at<double>(i,j) = Frame.at<double>(i,j);
							_variance_mm[min_arg].at<double>(i,j) = _initial_variance;

							// Renormalize weights
							double sum = 0;
							for(int k=0; k<_K; k++){
								sum += omega_mm_tmp[k].at<double>(i, j);
								}
							for(int k=0; k<_K; k++){
								omega_mm_tmp[k].at<double>(i, j) /= sum;
							}
							}
						}
					}
			}

			for(int k=0; k<_K; k++){
				_omega_mm[k] = omega_mm_tmp[k];}

			}
		}
}







