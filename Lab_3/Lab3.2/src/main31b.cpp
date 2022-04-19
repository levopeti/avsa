/* Applied Video Sequence Analysis - Escuela Politecnica Superior - Universidad Autonoma de Madrid
 *
 *	Starter code for Task 3.1b of the assignment "Lab3 - Kalman Filtering for object tracking"
 *
 *	This code has been tested using Ubuntu 18.04, OpenCV 3.4.4 & Eclipse 2019-12
 *
 * Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
 * Date: March 2022
 */
//includes
#include <opencv2/opencv.hpp> 	//opencv libraries
#include "kalman_filter.hpp"
#include "ShowManyImages.hpp"



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
	std::string inputvideo = "../../../Lab_3/dataset_lab3/lab3.1/singleball.mp4"; 	// path for the video to process


	//alternatively, the videofile can be passed via arguments of the executable
	if (argc == 3) inputvideo = argv[1];
	VideoCapture cap(inputvideo);


	//check if videofile exists
	if (!cap.isOpened())
		throw std::runtime_error("Could not open video file " + inputvideo); //throw error if not possible to read videofile



	//preload measurements from txt file
	std::ifstream ifile("../../../Lab_3/Lab3.2/src/meas_singleball.txt"); 	//filename with measurements (each line corresponds to X-Y coordinates of the measurement obtained for each frame)
	std::vector<cv::Point> measList; 					//variable where measurements will be stored
	std::string line; 									// auxiliary variable to read each line of file
	while (std::getline(ifile, line)) // read the current line
	{
		std::istringstream iss{line}; // construct a string stream from line

		// read the tokens from current line separated by comma
		std::vector<std::string> tokens; // here we store the tokens
		std::string token; // current token
		while (std::getline(iss, token, ' '))
			tokens.push_back(token); // add the token to the vector

		measList.push_back(cv::Point(std::stoi(tokens[0]),std::stoi(tokens[1])));
		//std::cout << "Processed point: " << std::stoi(tokens[0]) << " " << std::stoi(tokens[1]) << std::endl; //display read data
	}


	// KALMAN initialization
	int stateSize; //Dimensionality of the state (4: constant vel;  6: constant acc)

	// Input mode as argument
	if (argc == 2) stateSize = atoi(argv[1]);
	else stateSize = 4;

	KalmanFilter kf = create_kalman_filter(stateSize);

	// Vectors to store the measurements and estimations
	std::vector<cv::Point> v_estimations;
	std::vector<cv::Point> v_measurements;

	// Bool for the first detection
	bool first_detection = false;


	//main loop
	for (int i=0;true;i++) {

		// Get frame & check if we achieved the end of the videofile (e.g. frame.data is empty)
		cap >> frame;
		if (!frame.data)
			break;

		std::cout << "FRAME " << std::setfill('0') << std::setw(3) << ++count << std::endl; //print frame number

		// Get measurement from preloaded list of measurements
		cv::Point meas = measList[i];

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

		// imwrite({"../../../Lab_3/images/frame4_est" + str.str() + "_.png"}, frame_estimations);

		// cancel execution by pressing "ESC"
		if( (char)waitKey(100) == 27)
			break;
	}

	// Display trajectory
	VideoCapture cap2(inputvideo);
	cap2>>frame;
	
	draw_trajectory(v_estimations,"Estimations",frame,cvScalar(0,0,255),cvPoint(10,40));
	draw_trajectory(v_measurements,"Measurements",frame,cvScalar(0,255,0),cvPoint(10,20));

	namedWindow("Trajectory");
	imshow("Trajectory", frame);
	waitKey(0);


	printf("Finished program.\n");
	destroyAllWindows(); 	// closes all the windows
	return 0;
}



















