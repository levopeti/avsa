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
 Mat paintBlobImage(cv::Mat frame, std::vector<cvBlob> bloblist, bool labelled)
{
	cv::Mat blobImage;
	frame.copyTo(blobImage);


	//paint each blob of the list
	for(size_t i = 0; i < bloblist.size(); i++)
	{
		cvBlob blob = bloblist[i]; //get ith blob
		//...
		Scalar color;
		std::string label="";
		switch(blob.label){
		case PERSON:
			color = Scalar(255,0,0);
			label="PERSON";
			break;
		case CAR:
					color = Scalar(0,255,0);
					label="CAR";
					break;
		case OBJECT:
					color = Scalar(0,0,255);
					label="OBJECT";
					break;
		default:
			color = Scalar(255, 255, 255);
			label="UNKOWN";
		}

		Point p1 = Point(blob.x, blob.y);
		Point p2 = Point(blob.x+blob.w, blob.y+blob.h);

		rectangle(blobImage, p1, p2, color, 1, 8, 0);
		if (labelled)
			{
			rectangle(blobImage, p1, p2, color, 1, 8, 0);
			putText(blobImage, label, p1, FONT_HERSHEY_SIMPLEX, 0.5, color);
			}
			else
				rectangle(blobImage, p1, p2, Scalar(255, 255, 255), 1, 8, 0);
	}

	//return the image to show
	return blobImage;
}


/**
 *	Blob extraction from 1-channel image (binary). The extraction is performed based
 *	on the analysis of the connected components. All the input arguments must be
 *  initialized when using this function.
 *
 * \param fgmask Foreground/Background segmentation mask (1-channel binary image)
 * \param bloblist List with found blobs
 *
 * \return Operation code (negative if not succesfull operation)
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


int removeSmallBlobs(std::vector<cvBlob> bloblist_in, std::vector<cvBlob> &bloblist_out, int min_width, int min_height)
{

	//clear blob list (to fill with this function)
	bloblist_out.clear();

	// remove the blobs whose size is below some constraints
	for(size_t i = 0; i < bloblist_in.size(); i++)
	{
		cvBlob blob_in = bloblist_in[i]; //get ith blob
		if (blob_in.w > min_width && blob_in.h > min_height){
			bloblist_out.push_back(blob_in);
		}
	}

	//return OK code
	return 1;
}



