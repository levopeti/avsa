/* Applied Video Sequence Analysis - Escuela Politecnica Superior - Universidad Autonoma de Madrid
 *
 *	This source code belongs to the template (sample program)
 *	provided for the assignment LAB 4 "Histogram-based tracking"
 *
 *	This code has been tested using:
 *	- Operative System: Ubuntu 18.04
 *	- OpenCV version: 3.4.4
 *	- Eclipse version: 2019-12
 *
 * Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
 * Date: April 2020
 */
//includes
#include <stdio.h> 								//Standard I/O library
#include <numeric>								//For std::accumulate function
#include <string> 								//For std::to_string function
#include <opencv2/opencv.hpp>					//opencv libraries
#include "utils.hpp" 							//for functions readGroundTruthFile & estimateTrackingPerformance
#include "histogram.hpp"
#include "ShowManyImages.hpp"
#include "CandMatching.hpp"

//namespaces
using namespace cv;
using namespace std;



//main function
int main(int argc, char ** argv)
{
	//PLEASE CHANGE 'dataset_path' & 'output_path' ACCORDING TO YOUR PROJECT
	std::string dataset_path = "./dataset/";									//dataset location.
	std::string output_path = "./outvideos/";									//location to save output videos

	// // dataset paths
	// std::string sequences[] = {"bolt1",										//test data for lab4.1, 4.3 & 4.5
	// 						   "sphere","car1",								//test data for lab4.2
	// 						   "ball2","basketball",						//test data for lab4.4
	// 						   "bag","ball","road",};						//test data for lab4.6
	std::string sequences[] = {"ball2"};	

	std::string image_path = "%08d.jpg"; 									//format of frames. DO NOT CHANGE
	std::string groundtruth_file = "groundtruth.txt"; 						//file for ground truth data. DO NOT CHANGE
	int NumSeq = sizeof(sequences)/sizeof(sequences[0]);					//number of sequences

	// PARAMETERS CANDIDATE MATCHING
	vector<int> step_nums{10,11};
	vector<int> pixel_grid_stride{4, 2};

	// PARAMETERS HISTOGRAM
	int n_bins = 80;
	// PARAMETERS HOG
	Size s_block  = Size(0,0);
	Size s_cell   = Size(0,0);
	Size s_stride = Size(0,0);
	bool resize_img = false
	;
	
	// FEATURE SELECTION
	// GRAY_LEVEL = 0
	// HSV_H = 1
	// HSV_S = 2
	// RGB_R = 3
	// RGB_G = 4
    // RGB_B = 5
    // GRADIENT = 6

	int feat_val;
	CLASS feature;

	if (argc == 2) {
    	feat_val = atoi(argv[1]);
		if (feat_val>6 || feat_val<0)
		{	
			std::cout<<"-ERROR (the digit of the features seems to be incorrect)"<<std::endl;
			std::cout<<"-Ending script..."<<std::endl;
			exit(EXIT_SUCCESS);
		}
		feature = (CLASS)feat_val;
	}
	else{CLASS feature = RGB_R;}

	std::cout<<enum_str[feature]<<"\n";



	Mat hist_ff;
	

	//Loop for all sequence of each category
	for (int s=0; s<NumSeq; s++ )
	{
		Mat frame;										//current Frame
		int frame_idx=0;								//index of current Frame
		std::vector<Rect> list_bbox_est, list_bbox_gt;	//estimated & groundtruth bounding boxes
		std::vector<double> procTimes;					//vector to accumulate processing times

		std::string inputvideo = dataset_path + "/" + sequences[s] + "/img/" + image_path; //path of videofile. DO NOT CHANGE
		VideoCapture cap(inputvideo);	// reader to grab frames from videofile

		//check if videofile exists
		if (!cap.isOpened())
			throw std::runtime_error("Could not open video file " + inputvideo); //error if not possible to read videofile

		// Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file.
		cv::Size frame_size(cap.get(cv::CAP_PROP_FRAME_WIDTH),cap.get(cv::CAP_PROP_FRAME_HEIGHT));//cv::Size frame_size(700,460);
		VideoWriter outputvideo(output_path+"outvid_" + sequences[s]+".avi",CV_FOURCC('X','V','I','D'),10, frame_size);	//xvid compression (cannot be changed in OpenCV)

		//Read ground truth file and store bounding boxes
		std::string inputGroundtruth = dataset_path + "/" + sequences[s] + "/" + groundtruth_file;//path of groundtruth file. DO NOT CHANGE
		list_bbox_gt = readGroundTruthFile(inputGroundtruth); //read groundtruth bounding boxes

		//main loop for the sequence
		std::cout << "Displaying sequence at " << inputvideo << std::endl;
		std::cout << "  with groundtruth at " << inputGroundtruth << std::endl;
		int idd = 0;

		// mat for the first frame histogram
		Mat ff_hist;



		for (;;) {
			idd++;
			//get frame & check if we achieved the end of the videofile (e.g. frame.data is empty)
			cap >> frame;
			if (!frame.data)
				break;
				
			//Time measurement
			double t = (double)getTickCount();
			frame_idx=cap.get(cv::CAP_PROP_POS_FRAMES);			//get the current frame


			////////////////////////////////////////////////////////////////////////////////////////////

			//DO TRACKING

			// For the first frame only 
			if (idd==1)
			{	
				// get the first frame histogram and location
				ff_hist = create_histogram(frame, list_bbox_gt[frame_idx-1], n_bins, feature, s_block, s_cell, s_stride, resize_img);
				list_bbox_est.push_back(list_bbox_gt[frame_idx-1]);
			}

			
			// Generate the grif of candidates
			CandMatching cand_matching(list_bbox_est[frame_idx-1]);
			cand_matching.makeCandGrid(frame, step_nums, pixel_grid_stride);

			// Compare the candidates with the target 
			cand_matching.makeMatchScores(frame, ff_hist, n_bins, feature, s_block, s_cell, s_stride, resize_img);

			// Store the best candidate
			list_bbox_est.push_back(cand_matching.candWithMinScore());

			///////////////////////////////////////////////////

			//Time measurement
			procTimes.push_back(((double)getTickCount() - t)*1000. / cv::getTickFrequency());
			//std::cout << " processing time=" << procTimes[procTimes.size()-1] << " ms" << std::endl;

			// plot frame number & groundtruth bounding box for each frame
			putText(frame, std::to_string(frame_idx), cv::Point(10,15),FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255)); //text in red
			rectangle(frame, list_bbox_gt[frame_idx-1], Scalar(0, 255, 0));		//draw bounding box for groundtruth
			rectangle(frame, list_bbox_est[frame_idx], Scalar(255, 0, 0));		//draw bounding box for estimation

			// cout<<"ERROR"<<endl;

			// SHOW IMAGES
			ShowManyImages(enum_str[feature], 3, frame, frame(list_bbox_gt[frame_idx-1]), frame(list_bbox_est[frame_idx]));
			// ShowManyImages(enum_str[feature]+string("_fixed_bb"), 4, frame, frame(list_bbox_est[frame_idx-1]), img2, hist_img2);


			// imwrite("images/"+sequences[s]+std::to_string(idd)+"hist.jpg", hist_img);
			// imwrite("images/"+sequences[s]+std::to_string(idd)+"hist2.jpg", hist_img2);
			// imwrite("images/"+sequences[s]+std::to_string(idd)+"frame.jpg", frame);


			//show & save data
			// imshow("Tracking for "+sequences[s]+" (Green=GT, Red=Estimation)", frame);
			outputvideo.write(frame);//save frame to output video

			//exit if ESC key is pressed
			if(waitKey(30) == 27) break;
		}


		// break;

		//comparison groundtruth & estimation
		list_bbox_est.erase(list_bbox_est.begin());
		vector<float> trackPerf = estimateTrackingPerformance(list_bbox_gt, list_bbox_est);

		//print stats about processing time and tracking performance
		std::cout << "  Average processing time = " << std::accumulate( procTimes.begin(), procTimes.end(), 0.0) / procTimes.size() << " ms/frame" << std::endl;
		std::cout << "  Average tracking performance = " << std::accumulate( trackPerf.begin(), trackPerf.end(), 0.0) / trackPerf.size() << std::endl;

		//release all resources
		cap.release();			// close inputvideo
		outputvideo.release(); 	// close outputvideo
		destroyAllWindows(); 	// close all the windows
	}
	printf("Finished program. \n");
	return 0;
}
