/* Applied Video Analysis of Sequences (AVSA)
 *
 *	LAB2: Blob detection & classification
 *	Lab2.0: Sample Opencv project
 *
 *
 * Authors: José M. Martínez (josem.martinez@uam.es), Paula Moral (paula.moral@uam.es), Juan C. San Miguel (juancarlos.sanmiguel@uam.es)
 */

#include "blobs.hpp"
#include <algorithm>

/**
 *	Draws blobs with different rectangles on the image 'frame'. All the input arguments must be
 *  initialized when using this function.
 *
 * \param frame Input image
 * \param pBlobList List to store the blobs found
 * \param labelled - true write label and color bb, false does not wirite label nor color bb
 *
 * \return Image containing the draw blobs. If no blobs have to be painted
 *  or arguments are wrong, the function returns a copy of the original "frame".
 *
 */
 void paintCrossImage(cv::Mat &blobImage, cvBlob maxBlob, Scalar color)
{
	//cv::Mat blobImage;
	//frame.copyTo(blobImage);


	if (maxBlob.h > 0)
	{
		Point p1 = Point(maxBlob.x + (maxBlob.w / 2), maxBlob.y);
		Point p2 = Point(maxBlob.x + (maxBlob.w / 2), maxBlob.y + maxBlob.h);

		Point p3 = Point(maxBlob.x, maxBlob.y + (maxBlob.h / 2));
		Point p4 = Point(maxBlob.x + maxBlob.w, maxBlob.y + (maxBlob.h / 2));

		int thickness = 2;

		// Line drawn using 8 connected
		// Bresenham algorithm
		cv::line(blobImage, p1, p2, color, thickness, LINE_8);
		cv::line(blobImage, p3, p4, color, thickness, LINE_8);
	}

	//return the image to show
	//return blobImage;
}


/**
 *	Blob extraction from 1-channel image (binary). The extraction is performed based
 *	on the analysis of the connected components. All the input arguments must be
 *  initialized when using this function.
 *
 * \param fgmask Foreground/Background segmentation mask (1-channel binary image)
 * \param bloblist List with found blobs
 *
 * \return Operation code (negative if not successful operation)
 */
int extractBlobs(cv::Mat fgmask, std::vector<cvBlob> &bloblist, int connectivity)
{
	//check input conditions and return -1 if any is not satisfied
	if (connectivity != 4 && connectivity != 8)
	{
		return -1;
	}

	//required variables for connected component analysis
	Mat aux; // image to be updated each time a blob is detected (blob cleared)
	cv::Rect area;
	fgmask.convertTo(aux,CV_32SC1);

	//clear blob list
	bloblist.clear();

	//Connected component analysis
	for(int i=0; i<fgmask.rows; i++)
		for(int j=0; j<fgmask.cols; j++)
		{
			if (aux.at<int>(i, j) == 255){ // if foreground
				cv::floodFill(aux, cv::Point(j, i), 0, &area, cv::Scalar(), cv::Scalar(), connectivity); // algorithm
				cvBlob blob = initBlob(1, area.x, area.y, area.width, area.height); // create blob
				bloblist.push_back(blob); // store blob in list
			}

		}

	return 1;
}


cvBlob getBiggestBlob(std::vector<cvBlob> bloblist_in, int min_width, int min_height)
{
	//clear blob list (to fill with this function)
		cvBlob maxBlob = initBlob(0, 0, 0, 0, 0);
		int maxArea = 0;

		// remove the blobs whose size is below some constraints
		for(size_t i = 0; i < bloblist_in.size(); i++)
		{
			cvBlob blob_in = bloblist_in[i]; //get ith blob
			int area = blob_in.w * blob_in.h;
			if (area > maxArea && blob_in.w > min_width && blob_in.h > min_height){
				maxArea = area;
				maxBlob = blob_in;
			}
		}

		return maxBlob;
}





