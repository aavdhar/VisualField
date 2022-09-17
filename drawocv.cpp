#include <iostream>
#include <time.h>
#include "opencv2/core.hpp"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"






void myCircle(cv::Mat img, cv::Point center, int size, int B, int G, int R){
	cv::circle( img, center, size, cv::Scalar( B, G, R),cv::FILLED, cv::LINE_8 );
}

int main(int argc, char** argv)
{
 
    int x[40];
    int y[40];
    for (int i=0; i<40; i++){
	x[i] = rand()%(500-0 + 1) + 0;
    	y[i] = rand()%(500-0 + 1) + 0;
    }    
    // Create a blank image of size
    // (500 x 500) with black
    // background (B, G, R) : (0, 0, 0)
    cv::Mat image(500, 500, CV_8UC3,
              cv::Scalar(255, 255, 255));
  
    // Check if the image is created
    // successfully
    if (!image.data) {
        std::cout << "Could not open or find"
             << " the image";
  
        return 0;
    }
    int size = sizeof(x)/sizeof(x[0]);

    myCircle(image,cv::Point( 250, 250 ), 10, 0, 0, 255);
    imshow("window", image);
    cv::waitKey(1000);
    for (int i=0; i<size; i++){
    	myCircle(image,cv::Point(x[i],y[i]), 10, 170, 169, 173);
    	imshow("window", image);
    	cv::waitKey(1000);
    	myCircle(image,cv::Point(x[i], y[i]), 10, 255, 255 ,255);
    }
    return 0;
}


