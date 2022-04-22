/* Applied Video Sequence Analysis - Escuela Politecnica Superior - Universidad Autonoma de Madrid
 *
 *	Starter code for Task 3.1a of the assignment "Lab3 - Kalman Filtering for object tracking"
 *
 *	This code has been tested using Ubuntu 18.04, OpenCV 3.4.4 & Eclipse 2019-12
 *
 * Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
 * Date: March 2022
 */
//includes
#include <opencv2/opencv.hpp> 	//opencv libraries

//Header ShowManyImages
#include "ShowManyImages.hpp"

#include "kalman_filter.hpp"

//system libraries C/C++
#include <stdio.h>
#include <iostream>
#include <sstream>

//include for blob-related functions
#include "blobs.hpp"

//namespaces
using namespace cv; //avoid using 'cv' to declare OpenCV functions and variables (cv::Mat or Mat)
using namespace std;

//main function
int main(int argc, char ** argv)
{
	int count=0;		 											// frame counter
	Mat frame;														// frame of the video sequence
	Mat frame_estimations;
	Mat frame_measurements;
	std::string inputvideo = "/home/levcsi/IPCV_Madrid/AVSA/Lab_3/dataset_lab3/lab3.2/video6.mp4";
//	std::string inputvideo = "../../../Lab_3/dataset_lab3/lab3.1/singleball.mp4";

	//alternatively, the videofile can be passed via arguments of the executable
	if (argc == 3) inputvideo = argv[1];
	VideoCapture cap(inputvideo);							// reader to grab frames from video

	//check if videofile exists
	if (!cap.isOpened())
		throw std::runtime_error("Could not open video file " + inputvideo); //throw error if not possible to read videofile

	Ptr<cv::BackgroundSubtractorMOG2> mog = cv::createBackgroundSubtractorMOG2();
	mog->setHistory(50);
	mog->setVarThreshold(16.);

	Mat fgMOG;
	Mat efgMOG;
	Mat bgMOG;
	Mat GsFrame;
	Mat resultFrame;

	std::vector<cvBlob> trajectory;
	std::vector<cvBlob> bloblist;
	cvBlob maxBlob;

	double lr = 0.005;
	int min_width = 2;
	int min_height = 2;
	int morph_size = 1;


	// KALMAN initialization
	int stateSize = 6; //Dimensionality of the state (4: constant vel;  6: constant acc)

	// Input mode as argument
//	if (argc == 2) stateSize = atoi(argv[1]);
//	else stateSize = 4;

	KalmanFilter kf = create_kalman_filter(stateSize);

	// Vectors to store the measurements and estimations
	std::vector<cv::Point> v_estimations;
	std::vector<cv::Point> v_measurements;

	// Bool for the first detection
	bool first_detection = false;

	//main loop
	for (int i=0;true;i++) {

		std::cout << "FRAME " << std::setfill('0') << std::setw(3) << ++count << std::endl; //print frame number

		//get frame & check if we achieved the end of the videofile (e.g. frame.data is empty)
		cap >> frame;
		if (!frame.data)
			break;

		//convert frame to gray scale
		cvtColor(frame, GsFrame, COLOR_BGR2GRAY);

		//do measurement extraction
		mog->apply(GsFrame, fgMOG, lr);
		mog->getBackgroundImage(bgMOG);

		//morphological opening to filter noise
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));

		cv::erode(fgMOG, efgMOG, element, Point(-1, -1), 1);
		cv::dilate(efgMOG, efgMOG, element, Point(-1, -1), 1);

		//blob extraction
		extractBlobs(efgMOG, bloblist, 4);
		maxBlob = getBiggestBlob(bloblist, min_width, min_height);
//		frame.copyTo(resultFrame);
//		trajectory.push_back(maxBlob);

		// Get measurement from preloaded list of measurements
		cv::Point meas = Point(maxBlob.x + (maxBlob.w / 2), maxBlob.y + (maxBlob.h / 2));

		// Apply Kalman model
		do_kalman(meas, stateSize, kf, first_detection, v_estimations, v_measurements);

		// std::cout<<"x: "<<measurement(0)<<"  y: "<<measurement(1)<<endl;
		std::cout<<"x: "<<v_measurements[i].x<<"  y: "<<v_measurements[i].y<<endl;
		std::cout<<"x: "<<v_estimations[i].x<<"  y: "<<v_estimations[i].y<<endl;


		// display frame (YOU MAY ADD YOUR OWN VISUALIZATION FOR MEASUREMENTS, AND THE STAGES IMPLEMENTED)
		std::ostringstream str;
		str << std::setfill('0') << std::setw(3) << count;
		putText(frame,"Frame " + str.str(), cvPoint(30,30),FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,255,255), 1, CV_AA);
		frame.copyTo(frame_estimations);
		frame.copyTo(frame_measurements);
		drawMarker(frame_measurements, meas, cvScalar(0,255,0), MARKER_CROSS, 20,2); //display measurement
		drawMarker(frame_estimations, v_estimations[i], cvScalar(0,0,255), MARKER_CROSS, 20,2); //display estimation

		string title = "Input Image - Measurements - Estimations";
		ShowManyImages(title, 3, frame, frame_measurements, frame_estimations);

//		int a;
//		cin >> a;




//		// draw trajectory
//		for(size_t i = 0; i < trajectory.size(); i++)
//		{
//			paintCrossImage(resultFrame, trajectory[i], Scalar(0, 255, 0));
//		}
//
////		//display frame (YOU MAY ADD YOUR OWN VISUALIZATION FOR MEASUREMENTS, AND THE STAGES IMPLEMENTED)
////		std::ostringstream str;
////		str << std::setfill('0') << std::setw(3) << count;
////		putText(frame,"Frame " + str.str(), cvPoint(30,30),FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,255,255), 1, CV_AA);
////		imshow("Frame ",frame);
//
//		//SHOW RESULTS
//		//get the frame number and write it on the current frame
//		string title = " | Frame - GsFrame - BgM | FgM - eFgM - resultFrame";
//
//		ShowManyImages(title, 6,
//					   frame,
//				       GsFrame,
//					   bgMOG,
//					   fgMOG,
//					   efgMOG,
//					   resultFrame);

		//cancel execution by pressing "ESC"
		if( (char)waitKey(100) == 27)
			break;
	}

	printf("Finished program.");
	destroyAllWindows(); 	// closes all the windows
	return 0;
}
