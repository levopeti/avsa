#include <stdio.h> 								//Standard I/O library
#include <numeric>								//For std::accumulate function
#include <string> 								//For std::to_string function
#include <opencv2/opencv.hpp>					//opencv libraries
// #include "utils.hpp" 							//for functions readGroundTruthFile & estimateTrackingPerformance

//namespaces
using namespace cv;
using namespace std;

typedef enum {
	UNKNOWN=0,
	PERSON=1,
	GROUP=2,
	CAR=3,
	OBJECT=4
} CLASS;

using namespace std;
using namespace cv;

//main function
int main(int argc, char ** argv)
{
    Mat a = (cv::Mat_<float>(1,12) << 
        1,  0,  0,  1,  0,  0, 
        0,  0,  0,  1,  0,  0);

	Mat b = a.reshape(0, 1);
	cout<<b.size()<<"\n";
	cout<<a<<"\n";

	cout<<b<<"\n";
	cv::reduce(b, b, 0, CV_REDUCE_SUM, -1);
	b = b.reshape(0, 12);

	cout<<b<<"\n";
	cout<<b.size()<<"\n";





}