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



 /**
  *	Blob classification between the available classes in 'Blob.hpp' (see CLASS typedef). All the input arguments must be
  *  initialized when using this function.
  *
  * \param frame Input image
  * \param fgmask Foreground/Background segmentation mask (1-channel binary image)
  * \param bloblist List with found blobs
  *
  * \return Operation code (negative if not succesfull operation)
  */

 // ASPECT RATIO MODELS
#define MEAN_PERSON 0.3950
#define STD_PERSON 0.1887

#define MEAN_CAR 1.4736
#define STD_CAR 0.2329

#define MEAN_OBJECT 1.2111
#define STD_OBJECT 0.4470

// end ASPECT RATIO MODELS

// distances
float ED(float val1, float val2)
{
	return sqrt(pow(val1-val2,2));
}

float WED(float val1, float val2, float std)
{
	return sqrt(pow(val1-val2,2)/pow(std,2));
}
//end distances
 int classifyBlobs(std::vector<cvBlob> &bloblist)
 {

 	//required variables for classification
	 float aspect_ratio;
	 double min_distance;
	 int argMin;


 	//classify each blob of the list
 	for(size_t i = 0; i < bloblist.size(); i++)
 	{
 		argMin = 0;
 		cvBlob blob = bloblist[i]; //get i-th blob
 		aspect_ratio = (float) blob.w / (float) blob.h; // compute blob's aspect ratio

 		// ED
 		float dist_person = ED(aspect_ratio, MEAN_PERSON);
 		float dist_car = ED(aspect_ratio, MEAN_CAR);
 		float dist_object = ED(aspect_ratio, MEAN_OBJECT);

 		// WED
// 		float dist_person = WED(aspect_ratio, MEAN_PERSON, STD_PERSON);
// 		float dist_car = WED(aspect_ratio, MEAN_CAR, STD_CAR);
// 		float dist_object = WED(aspect_ratio, MEAN_OBJECT, STD_OBJECT);

 		std::vector<float> v  = {dist_person, dist_car, dist_object}; // Distances vector
 		std::vector<bool>  vb = {dist_person<STD_PERSON, dist_car<STD_CAR, dist_object<STD_OBJECT}; // Vector "BELONG OR NOT" to Gaussian
 		min_distance = *std::max_element(v.begin(), v.end());

 		// For those who belong to the gaussian, check who has the minumum deviation
 		for (int n=0; n<3; n++)
 			if(vb[n] && v[n]<=min_distance) {min_distance = v[n]; argMin = n+1;}

 		// Label
 		if (argMin){
 			switch(argMin){
 					case 1:
 						blob.label = PERSON;
 						break;
 					case 2:
 						blob.label = CAR;
						break;
 					case 3:
 						blob.label = OBJECT;
						break;
 			}
 		}
 		bloblist[i] = blob;
 	}
 	return 1;
 }

//stationary blob extraction function
 /**
  *	Stationary FG detection
  *
  * \param fgmask Foreground/Background segmentation mask (1-channel binary image)
  * \param fgmask_history Foreground history counter image (1-channel integer image)
  * \param sfgmask Foreground/Background segmentation mask (1-channel binary image)
  *
  * \return Operation code (negative if not succesfull operation)
  *
  *
  * Based on: Stationary foreground detection for video-surveillance based on foreground and motion history images, D.Ortego, J.C.SanMiguel, AVSS2013
  *
  */

#define FPS 25 //check in video - not really critical
#define SECS_STATIONARY 0 // to set
#define I_COST 0 // to set // increment cost for stationarity detection
#define D_COST 0 // to set // decrement cost for stationarity detection
#define STAT_TH 0.0 // to set

 int extractStationaryFG (Mat fgmask, Mat &fgmask_history, Mat &sfgmask)
 {

//	 int numframes4static=(int)(FPS*SECS_STATIONARY);


	 // update fgmask_counter
	 for (int i=0; i<fgmask.rows;i++)
		 for(int j=0; j<fgmask.cols;j++)
		 {
			// ...
			 fgmask_history.at<float>(i,j) = 0; // void implementation (no history)
		 }//for

	// update sfgmask
	for (int i=0; i<fgmask.rows;i++)
		 for(int j=0; j<fgmask.cols;j++)
			 {
			 	 // ...
				 sfgmask.at<uchar>(i,j)=0;// void implementation (no stationary fg)
			 }
 return 1;
 }


