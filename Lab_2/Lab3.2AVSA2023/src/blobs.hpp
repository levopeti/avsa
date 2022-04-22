/* Applied Video Analysis of Sequences (AVSA)
 *
 *	LAB2: Blob detection & classification
 *	Lab2.0: Sample Opencv project
 *
 *
 * Authors: José M. Martínez (josem.martinez@uam.es), Paula Moral (paula.moral@uam.es), Juan C. San Miguel (juancarlos.sanmiguel@uam.es)
 */

 //class description
/**
 * \class BasicBlob
 * \brief Class to describe a basic blob and associated functions
 *
 *
 */

#ifndef BLOBS_H_INCLUDE
#define BLOBS_H_INCLUDE

#include "opencv2/opencv.hpp"
using namespace cv; //avoid using 'cv' to declare OpenCV functions and variables (cv::Mat or Mat)


// Maximun number of char in the blob's format
const int MAX_FORMAT = 1024;


struct cvBlob {
	int     ID;  /* blob ID        */
	int   x, y;  /* blob position  */
	int   w, h;  /* blob sizes     */
	char format[MAX_FORMAT];
};

inline cvBlob initBlob(int id, int x, int y, int w, int h)
{
	cvBlob B = {id, x, y, w, h};
	return B;
}

/*
* Headers of blob-based functions
*
*/

//blob drawing functions
void paintCrossImage(Mat &blobImage, cvBlob maxBlob, Scalar color);

//blob extraction functions
int extractBlobs(Mat fgmask, std::vector<cvBlob> &bloblist, int connectivity);

//find biggest blob above the thresholds
cvBlob getBiggestBlob(std::vector<cvBlob> bloblist_in, int min_width, int min_height);


#endif
