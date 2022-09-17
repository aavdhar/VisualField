#include <iostream>
#include <time.h>
#include "opencv2/core.hpp"
#include <chrono>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;
using namespace std::chrono;


void myCircle(Mat img, Point center, int size, int B, int G, int R){
	circle( img, center, size, Scalar( B, G, R), FILLED, LINE_8 );
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
    Mat image(500, 500, CV_8UC3,
              Scalar(255, 255, 255));
  
    // Check if the image is created
    // successfully
    if (!image.data) {
        cout << "Could not open or find"
             << " the image";
  
        return 0;
    }
    int size = sizeof(x)/sizeof(x[0]);
    myCircle(image, Point( 250, 250 ), 10, 0, 0, 255);
    imshow("window", image);
    waitKey(1000);
    for (int i=0; i<size; i++){
    	myCircle(image, Point(x[i],y[i]), 10, 170, 169, 173);
    	imshow("window", image);
    	waitKey(1000);
    	myCircle(image, Point(x[i], y[i]), 10, 255, 255 ,255);
    }
    return 0;
}


