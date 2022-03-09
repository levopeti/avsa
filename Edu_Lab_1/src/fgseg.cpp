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
bgs::bgs(double th, double alpha, bool selective_update, int threshold_ghosts2, bool rgb, bool blind_mode)
{
	_rgb=rgb;

	_threshold=th;

	_alpha=alpha;

	_selective_update=selective_update;

	_threshold_ghosts2=threshold_ghosts2;

	_blind_mode = blind_mode;
}

//default destructor
bgs::~bgs(void)
{
}

//method to initialize bkg (first frame - hot start)
void bgs::init_bkg(cv::Mat Frame)
{


	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color

		_bkg = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		_inter = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		Frame.copyTo(_bkg);

	}
	else{

		_bkg = Mat(Frame.cols,Frame.rows, CV_8UC3, Scalar::all(0)); //CV_8UC3
		_inter = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); //additional mat use as counter
		Frame.copyTo(_bkg);


	}

}

//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		Frame.copyTo(_frame);


		_diff = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		_bgsmask = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix

		//EXERCISE 1
		absdiff(_frame,_bkg,_diff);
		threshold(_diff,_bgsmask,_threshold,255,0);

		//EXERCISE 2
		if (_selective_update)
		{
			for (int i=0; i<_bgsmask.rows; i++){
				for (int j=0; j<_bgsmask.cols; j++){
					if (_bgsmask.at<uchar>(i,j)  == 0){ //if the pixel belongs to the background we update it
						_bkg.at<uchar>(i,j) = (_alpha * _frame.at<uchar>(i,j)) + ((1 - _alpha) * _bkg.at<uchar>(i,j));
					}
				}
			}
		}
		else //blind mode (we update all the pixels, do not care if they belong to the background or not)
		{
			if (_blind_mode){
				_bkg = (_alpha * _frame) + ((1 - _alpha) * _bkg);
			}

		}

		//EXERCISE 3
		for (int i=0; i<_bgsmask.rows; i++){
			for (int j=0; j<_bgsmask.cols; j++){
				if (_bgsmask.at<uchar>(i,j) > 0){
					_inter.at<uchar>(i,j) = _inter.at<uchar>(i,j) + 1; //Increase the counter
					if (_inter.at<uchar>(i,j) >_threshold_ghosts2){
						_bkg.at<uchar>(i,j) = _frame.at<uchar>(i,j); //Update the background
					}
				}
				else{
					_inter.at<uchar>(i,j) = 0; //Reset the counter
				}
			}
		}


		//...
	}
	else{

		Frame.copyTo(_frame);

		_diff = Mat(Frame.cols,Frame.rows, CV_8UC3, Scalar::all(0));
		_bgsmask = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);


		Mat bgsmask3 = Mat(Frame.cols, Frame.rows, CV_8UC3, Scalar::all(0));
		Mat rgbimg[3];


		//EXERCISE 1
		//Compute difference and threshold in 3 channel matrix
		absdiff(_frame,_bkg,_diff);
		threshold(_diff,bgsmask3,_threshold,255,0);

		//Split the three channels matrix and perform logic OR to calculate _bgsmask (max value 255)
		split(bgsmask3,rgbimg);
		_bgsmask = rgbimg[0] + rgbimg[1] + rgbimg[2];



		//EXERCISE 2
		if (_selective_update)
		{
			for (int i=0; i<_bgsmask.rows; i++){
				for (int j=0; j<_bgsmask.cols; j++){
					if (_bgsmask.at<uchar>(i,j) == 0){ //if the pixel belongs to the background we update it
						_bkg.at<Vec3b>(i,j) = (_alpha * _frame.at<Vec3b>(i,j)) + ((1 - _alpha) * _bkg.at<Vec3b>(i,j));
					}
				}
			}
		}
		else //blind mode (we update all the pixels, do not care if they belong to the background or not)
		{
			if (_blind_mode){
				_bkg = (_alpha * _frame) + ((1 - _alpha) * _bkg);
			}

		}


		//EXERCISE 3
		for (int i=0; i<_bgsmask.rows; i++){
			for (int j=0; j<_bgsmask.cols; j++){
				if (_bgsmask.at<uchar>(i,j) > 0){
					_inter.at<uchar>(i,j) = _inter.at<uchar>(i,j) + 1; //Increase the counter
					if (_inter.at<uchar>(i,j) >_threshold_ghosts2){
						_bkg.at<Vec3b>(i,j) = _frame.at<Vec3b>(i,j); //Update the background
					}
				}
				else{
					_inter.at<uchar>(i,j) = 0;//Reset the counter
				}
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
	absdiff(_bgsmask, _bgsmask, _shadowmask);// currently void function mask=0 (should create shadow mask)


//	_bgsmask = _bgsmask * 255;

	absdiff(_bgsmask, _shadowmask, _fgmask); // eliminates shadows from bgsmask
}

//ADD ADDITIONAL FUNCTIONS HERE




