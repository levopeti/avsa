/* Applied Video Sequence Analysis (AVSA)
 *
 *	LAB1.0: Background Subtraction - Unix version
 *	fgesg.hpp
 *
 * 	Authors: José M. Martínez (josem.martinez@uam.es), Paula Moral (paula.moral@uam.es) & Juan Carlos San Miguel (juancarlos.sanmiguel@uam.es)
 *	VPULab-UAM 2020
 */


#include <opencv2/opencv.hpp>

#ifndef FGSEG_H_INCLUDE
#define FGSEG_H_INCLUDE

using namespace cv;
using namespace std;

namespace fgseg {


	//Declaration of FGSeg class based on BackGround Subtraction (bgs)
	class bgs{
	public:

		//constructor with parameter "threshold"
		bgs(double threshold, double alpha, bool selective_bkg_update, int threshold_ghosts2, bool rgb,
				double alpha_sh, double beta_sh, double saturation_th, double hue_th, double sigma_coef,
				bool unimodal);

		//destructor
		~bgs(void);

		//method to initialize bkg (first frame - hot start)
		void init_bkg(cv::Mat Frame);

		//method to perform BackGroundSubtraction
		void bkgSubtraction(cv::Mat Frame);

		//method to detect and remove shadows in the binary BGS mask
		void removeShadows();

		//unimodal gaussian
		void updateGaussian(cv::Mat Frame, int frame_idx);

		//returns the BG image
		cv::Mat getBG(){return _bkg;};

		//returns the DIFF image
		cv::Mat getDiff(){return _diff;};

		//returns the BGS mask
		cv::Mat getBGSmask(){return _bgsmask;};

		//returns the binary mask with detected shadows
		cv::Mat getShadowMask(){return _shadowmask;};

		//returns the binary FG mask
		cv::Mat getFGmask(){return _fgmask;};


		//ADD ADITIONAL METHODS HERE
		//...
	private:
		cv::Mat _bkg; //Background model
		cv::Mat	_frame; //current frame
		cv::Mat _diff; //abs diff frame
		cv::Mat _bgsmask; //binary image for bgssub (FG)
		cv::Mat _shadowmask; //binary mask for detected shadows
		cv::Mat _fgmask; //binary image for foreground (FG)

		cv::Mat _fgcounter; //foreground counter per pixel

		cv::Mat _mean; //mean value per pixel
		cv::Mat _variance; //variance per pixel
		cv::Mat _sum; //sum of pixels
		cv::Mat _sum_squares; //sum of square of pixels

		std::vector<cv::Mat> _mean_mm; //mean value per pixel
		std::vector<cv::Mat> _variance_mm; //variance per pixel
		std::vector<cv::Mat> _omega_mm; //number of pixel within the gaussian's interval


		bool _rgb;
		double _threshold;
		double _alpha;
		bool _selective_bkg_update;
		int _threshold_ghosts2;

		double _alpha_sh;
		double _beta_sh;
		int _saturation_th;
		int _hue_th;

		double _sigma_coef;
		bool _unimodal;

	};//end of class bgs

}//end of namespace

#endif




