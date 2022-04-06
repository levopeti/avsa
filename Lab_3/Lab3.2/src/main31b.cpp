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



//namespaces
using namespace cv; //avoid using 'cv' to declare OpenCV functions and variables (cv::Mat or Mat)
using namespace std;

void draw_lines_points(std::vector<cv::Point> v, Mat frame, CvScalar color);

//main function
int main(int argc, char ** argv)
{
	int count=0;		 											// frame counter
	Mat frame;														// frame of the video sequence
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
	int stateSize = 4; //Dimensionality of the state
	int measSize  = 2; //Dimensionality of the measurement
	int contrSize = 0; //Dimensionality of the control vector

	unsigned int type = CV_32F;
	cv::KalmanFilter kf(stateSize, measSize, contrSize, type);

	cv::Mat state(stateSize, 1, type);
	Mat_<float>measurement(2,1);
//	cv::Mat measurement(2,1,type);
	measurement.setTo(Scalar(0));
	Mat estimated;

	// A
	setIdentity(kf.transitionMatrix);
	kf.transitionMatrix.at<float>(0,1)=1; kf.transitionMatrix.at<float>(2,3)=1;

	// H
	kf.measurementMatrix = (Mat_<float>(measSize,stateSize) << 1,0,0,0, 0,0,1,0);

	// Q
	kf.processNoiseCov = (Mat_<float>(stateSize,stateSize) << 25,0,0,0, 0,10,0,0,  0,0,25,0,  0,0,0,10);

	// R
	setIdentity(kf.measurementNoiseCov, Scalar::all(25));

	// P
	setIdentity(kf.errorCovPost, Scalar::all(10e5));

//	kf.statePre.at<float>(0) = mouse_info.x;

	// First state
//	std::cout<<kf.statePre<<endl;


	std::vector<cv::Point> v_estimations;
	std::vector<cv::Point> v_measurements;
	bool first_detection = false;


	//main loop
	for (int i=0;true;i++) {

		//get frame & check if we achieved the end of the videofile (e.g. frame.data is empty)
		cap >> frame;
		if (!frame.data)
			break;

		std::cout << "FRAME " << std::setfill('0') << std::setw(3) << ++count << std::endl; //print frame number

		//get measurement from preloaded list of measurements
		cv::Point meas = measList[i];


		// PREDICTION
		state = kf.predict();

		// Measurements
		measurement(0) = meas.x;
		measurement(1) = meas.y;

		// First time the ball is detected
		if (meas.x>0 && meas.y>0 && first_detection == false)
		{
			state.at<float>(0) = meas.x;
			state.at<float>(2) = meas.y;
			kf.statePost = state;

			first_detection = true;
		}

		// If there are no observations
		if (meas.x<0 || meas.y<0)
		{
			estimated = state;
			kf.statePost = state;
		}
		else
		{
			estimated = kf.correct(measurement);
		}

		std::cout<<"x: "<<measurement(0)<<"  y: "<<measurement(1)<<endl;
		std::cout<<"x: "<<estimated.at<float>(0)<<"  y: "<<estimated.at<float>(2)<<endl;

		// Store the measurements and estimations in a vector
		v_measurements.push_back(meas);
		v_estimations.push_back(cv::Point(estimated.at<float>(0),estimated.at<float>(2)));


		//display frame (YOU MAY ADD YOUR OWN VISUALIZATION FOR MEASUREMENTS, AND THE STAGES IMPLEMENTED)
		std::ostringstream str;
		str << std::setfill('0') << std::setw(3) << count;
		putText(frame,"Frame " + str.str(), cvPoint(30,30),FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,255,255), 1, CV_AA);
		drawMarker(frame, meas, cvScalar(255,0,0), MARKER_CROSS, 20,2); //display measurement
		drawMarker(frame, v_estimations[i], cvScalar(0,0,255), MARKER_CROSS, 20,2); //display measurement
		imshow("Frame ",frame);

		//cancel execution by pressing "ESC"
		if( (char)waitKey(100) == 27)
			break;
	}

	// Display trajectory
	VideoCapture cap2(inputvideo);

	cap2>>frame;
	namedWindow("Trajectory");

	putText(frame,"Measurements",cvPoint(10,20),FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,255,0), 1, CV_AA);
	putText(frame,"Estimations",cvPoint(10,40),FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,255), 1, CV_AA);


	draw_lines_points(v_estimations,frame,cvScalar(0,0,255));
	draw_lines_points(v_measurements,frame,cvScalar(0,255,0));

	imshow("Trajectory", frame);
	waitKey(0);


	printf("Finished program.\n");
	destroyAllWindows(); 	// closes all the windows
	return 0;
}

// Function to draw the trajectory in the final image
void draw_lines_points(std::vector<cv::Point> v, Mat frame, CvScalar color)
{

	for (size_t i=1; i<v.size()-1; i++)
	{
		if (v[i].x>0 && v[i].y>0)
		{
			cv::circle(frame, v[i], 3, color, CV_FILLED);
			if (v[i+1].x>0 && v[i+1].y>0)
			{
				cv::line(frame, v[i], v[i+1], color);
			}
		}
	}

}

















