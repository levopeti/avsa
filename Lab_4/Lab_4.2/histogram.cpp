#include "histogram.hpp"

int MEA_NOISE_COV = 10;
int ERROR_COV_POST = 1e5;


cv::Mat create_histogram(cv::Mat Frame, cv::Rect Bb, int bins, CLASS feature, cv::Size s_block, cv::Size s_cell, cv::Size s_stride, bool resize_img)
{   
    // get bounding box region from the input image
    Frame = Frame(Bb).clone();

    // Initialization
    cv::Mat img;
    cv::Mat channels[3];  
    cv::Mat b_hist;
    cv::Mat des;

 


    switch(feature) {
        case 0: // Grayscale
            cvtColor(Frame, img, cv::COLOR_BGR2GRAY);
        break;
        case 1: // H
            cvtColor(Frame, Frame, cv::COLOR_BGR2HSV);
            split(Frame, channels);
            img = channels[0];
        break;
        case 2: // S
            cvtColor(Frame, Frame, cv::COLOR_BGR2HSV);
            split(Frame, channels);
            img = channels[1];
        break;
        case 3: // R
            split(Frame, channels);
            img = channels[2];
        break;
        case 4: // G
            split(Frame, channels);
            img = channels[1];
        break;
        case 5: // B
            split(Frame, channels);
            img = channels[0];
        break;
        case 6: // gradients
            cvtColor(Frame, img, cv::COLOR_BGR2GRAY);
            des = computeHOGs(img, bins, s_block, s_cell, s_stride, resize_img);
            des.copyTo(b_hist);
        break;
        default:
            std::cout<<"-ERROR (the name of the features seems to be incorrect)"<<std::endl;
            std::cout<<"-Ending script..."<<std::endl;
            exit(EXIT_SUCCESS);

}



// Range of the histogram
float range[2] = {};
range[0] = 0;

if (feature==1) {
    range[1] = 181;
    }
else {
    range[1] = 256;
    }

const float* histRange = {range};


// Calculate the histogram (if not gradient)
if (feature!=6)
{
    cv::calcHist(&img, 1, 0, cv::Mat(), b_hist, 1, &bins, &histRange, true, false);
}


// Create image to visualize the histogram
// int hist_w = 512, hist_h = 400;
// int bin_w = cvRound((double) hist_w/(bins-1));
// cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255,255,255));

// NORMALIZE the histogram (within min and max value)
// cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
cv::normalize(b_hist, b_hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

// // Draw the histogram in the image
// for( int i = 1; i < bins; i++)
// {
//     line(histImage, cv::Point(bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1))),
//             cv::Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
//             cv::Scalar(255, 0, 0), 2, 8, 0);
// }

// return image to visualize histogram
// return histImage;
return b_hist;
}



cv::Mat computeHOGs(cv::Mat gray, int bins, cv::Size s_block, cv::Size s_cell, cv::Size s_stride, bool resize_img)
{   

    cv::HOGDescriptor hog;
    std::vector<float> descriptors;


    // If we want to resize the crop to use common block/cell sizes (size not fixed)
    // block = (16,16); cell = (8,8); stride = (8,8)
    if (resize_img)
    {
        int row_size = gray.rows + (8 - gray.rows%8);
        int col_size = gray.cols + (8 - gray.cols%8);
        cv::resize(gray, gray, cv::Size(col_size,row_size), 0, 0, cv::INTER_LINEAR_EXACT);
    }

    // either: 1.) custom sizes
    //         2.) block = gray.size(); cell = gray.size(); stride = (1,1)
    else
    {
        // blocksize
        if (s_block==cv::Size(0,0)){hog.blockSize = gray.size();}
        else {hog.blockSize = s_block;}

        // cellsize (usually 4 times smaller than blocksize (i.e 16x16 => 8x8))
        if (s_cell==cv::Size(0,0)){hog.cellSize = gray.size();}
        else {hog.cellSize = s_cell;}
    
        // stride
        if (s_stride==cv::Size(0,0)){hog.blockStride = cv::Size(1,1);}
        else {hog.blockStride = s_stride;}
    }
    
    // std::cout<<hog.blockSize<<"\n";
    // std::cout<<hog.cellSize<<"\n";
    // std::cout<<hog.blockStride<<"\n";



    // input image size
    hog.winSize = gray.size();
    
    // number of bins
    hog.nbins = bins;


    // compute the descriptors
    hog.compute(gray, descriptors, cv::Size(0,0), cv::Size(0,0));
    // transform to mat
    cv::Mat output = cv::Mat(descriptors).clone();

    // merge all histograms in one (we could stay with all of them, some constraint needed)
    if (descriptors.size()!=bins)
    {
        output = output.reshape(0, descriptors.size()/bins);
	    cv::reduce(output, output, 0, CV_REDUCE_SUM, -1);
        output = output.reshape(0, bins);
    }
    
    return output;   
}

