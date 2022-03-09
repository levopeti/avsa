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

using namespace fgseg;

//default constructor
bgs::bgs(double threshold, double alpha, bool selective_bkg_update, int threshold_ghosts2, bool rgb)
{
	_threshold=threshold;
	_alpha=alpha;
	_rgb=rgb;
	_selective_bkg_update=selective_bkg_update;
	_threshold_ghosts2=threshold_ghosts2;
}

//default destructor
bgs::~bgs(void)
{
}

//method to initialize bkg (first frame - hot start)
void bgs::init_bkg(cv::Mat Frame)
{

	if (!_rgb)
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color

	_bkg = Frame.clone();
	_fgcounter = Mat::zeros(Size(Frame.cols, Frame.rows), CV_16UC1);
}

//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{

	if (!_rgb){
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

		absdiff(_bkg, Frame, _diff);
		threshold(_diff, bgsmask_rgb, _threshold, 255, cv::THRESH_BINARY);

		for(int j=0; j<bgsmask_rgb.rows; ++j)
			for(int i=0; i<bgsmask_rgb.cols; ++i)
			{
				if (bgsmask_rgb.at<cv::Vec3b>(j, i)[0] || bgsmask_rgb.at<cv::Vec3b>(j, i)[1] || bgsmask_rgb.at<cv::Vec3b>(j, i)[2])
					_bgsmask.at<uchar>(j, i) = 255;
			}

		if (_selective_bkg_update)
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
	// init Shadow Mask (currently Shadow Detection not implemented)
	_bgsmask.copyTo(_shadowmask); // creates the mask (currently with bgs)

	//ADD YOUR CODE HERE
	//...
	absdiff(_bgsmask, _bgsmask, _shadowmask);// currently void function mask=0 (should create shadow mask)
	//...

	absdiff(_bgsmask, _shadowmask, _fgmask); // eliminates shadows from bgsmask
}

//ADD ADDITIONAL FUNCTIONS HERE




