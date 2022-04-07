#include "kalman_filter.hpp"

cv::KalmanFilter create_kalman_filter(int stateSize)
{
    int measSize = 2; //Dimensionality of the measurement
    cv::KalmanFilter kf(stateSize, measSize, 0, CV_32F);

    if(stateSize==4)
    {   
        // A
        kf.transitionMatrix = (cv::Mat_<float>(stateSize,stateSize) << 
        1,  1,  0,  0, 
        0,  1,  0,  0,  
        0,  0,  1,  1,  
        0,  0,  0,  1);

        // H
        kf.measurementMatrix = (cv::Mat_<float>(measSize,stateSize) << 
        1,  0,  0,  0,
        0,  0,  1,  0);

        // Q
        kf.processNoiseCov = (cv::Mat_<float>(stateSize,stateSize) << 
        25,  0,  0,  0, 
         0, 10,  0,  0,  
         0,  0, 25,  0,  
         0,  0,  0, 10);

        // R and P
        setIdentity(kf.measurementNoiseCov, cv::Scalar::all(25));
        setIdentity(kf.errorCovPost, cv::Scalar::all(10e5));



    }
    else if (stateSize==6)
    {   
        // A
        kf.transitionMatrix = (cv::Mat_<float>(stateSize,stateSize) << 
        1,  1,1/2,  0,  0,  0, 
        0,  1,  1,  0,  0,  0,  
        0,  0,  1,  0,  0,  0,  
        0,  0,  0,  1,  1,1/2,
        0,  0,  0,  0,  1,  1,
        0,  0,  0,  0,  0,  1);

        // H
        kf.measurementMatrix = (cv::Mat_<float>(measSize,stateSize) << 
	    1,  0,  0,  0,  0,  0, 
	    0,  0,  0,  1,  0,  0);

        // Q
        kf.processNoiseCov = (cv::Mat_<float>(stateSize,stateSize) << 
        25,  0,  0,  0,  0,  0, 
         0, 10,  0,  0,  0,  0,   
         0,  0,  1,  0,  0,  0,   
         0,  0,  0, 25,  0,  0, 
         0,  0,  0,  0, 10,  0,
         0,  0,  0,  0,  0,  1);
       
       	// R and P
	    setIdentity(kf.measurementNoiseCov, cv::Scalar::all(25));
	    setIdentity(kf.errorCovPost, cv::Scalar::all(10e5));

    }
    else
    {
        std::cout<<"-ERROR (the model has not been implemented)"<<std::endl;
        std::cout<<"-Ending script..."<<std::endl;
        exit(EXIT_SUCCESS);
    }
    
    return kf;
    
}


void do_kalman(cv::Point meas, int stateSize, cv::KalmanFilter &kf, bool &first_detection, std::vector<cv::Point> &v_estimations, std::vector<cv::Point> &v_measurements)
{

    // Initialization
    cv::Mat state(stateSize, 1, CV_32F);
	cv::Mat estimated(stateSize, 1, CV_32F);
	cv::Mat measurement(2, 1, CV_32F);

    // PREDICTION
    state = kf.predict();

    // Measurements
    measurement.at<float>(0) = meas.x;
    measurement.at<float>(1) = meas.y;

    // Until the ball is detected once
    if (first_detection == false)
    {
        state.at<float>(0) = meas.x;
        state.at<float>(stateSize/2) = meas.y;
        kf.statePost = state;
        // First time the ball is detected
        if (meas.x>0 && meas.y>0)
            first_detection = true;
    }
    // If there are no observations
    if (meas.x<0 || meas.y<0)
    {
        kf.statePost = state;
    }
    // UPDATE
    else
    {
        kf.correct(measurement);
    }

    // Store the measurements and estimations in a vector
    v_measurements.push_back(meas);
    v_estimations.push_back(cv::Point(kf.statePost.at<float>(0),kf.statePost.at<float>(stateSize/2)));

}


void draw_trajectory(std::vector<cv::Point> v, cv::String legend, cv::Mat &frame, CvScalar color, cv::Point origin_point)
{

    // Write the legend
	putText(frame,legend,origin_point,cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, color, 1, CV_AA);

    // Draw the trayectory and the points
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


