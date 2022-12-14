#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <thread>
#include <iostream>
#include <chrono>
#include <queue>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <fstream>

#include "constants.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"

/* Attempt at supporting openCV version 4.0.1 or higher */
#if CV_MAJOR_VERSION >= 4
#define CV_WINDOW_NORMAL                cv::WINDOW_NORMAL
#define CV_BGR2YCrCb                    cv::COLOR_BGR2YCrCb
#define CV_HAAR_SCALE_IMAGE             cv::CASCADE_SCALE_IMAGE
#define CV_HAAR_FIND_BIGGEST_OBJECT     cv::CASCADE_FIND_BIGGEST_OBJECT
#endif

using namespace std;


/** Constants **/


/** Function Headers */
void detectAndDisplay( cv::Mat frame );

/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
cv::String face_cascade_name = "../res/haarcascade_frontalface_alt.xml";
cv::CascadeClassifier face_cascade;
std::string main_window_name = "Capture - Face detection";
std::string face_window_name = "Capture - Face";
cv::RNG rng(12345);
cv::Mat debugImage;
cv::Mat skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);

/**
 * @function main
 */

void dotMover(std::string msg);
void pupilTracker(std::string msg);

bool 
stdThreadSetPriority(int priority)
{
    struct sched_param thread_param; 
    int    policy;

    pthread_getschedparam(pthread_self(), &policy, &thread_param);

std::cout << "Thread Handle: " << pthread_self() << ", Policy: " << policy << ", Priority: " << thread_param.sched_priority << endl;
std::cout << endl;

    thread_param.sched_priority = priority; 

    pthread_setschedparam(pthread_self(), SCHED_RR, &thread_param);
    return true;
}


int 
main( int argc, const char** argv ) 
{
#ifdef NOTHREADING
    pupilTracker("Hello Pupil Tracker");
#else
    std::thread t1(pupilTracker, "Hello Pupil Tracker");

    t1.detach();

    dotMover("Hello Dot Mover");
#endif

    return 0;
}

void 
pupilTracker(std::string msg) 
{

    stdThreadSetPriority(31);  // Setting highest priority in NORMAL scheduling

    cv::Mat frame;

  // Load the cascades
    if(!face_cascade.load( face_cascade_name ))
    { 
        std::cout << "--(!)Error loading face cascade, please change face_cascade_name in source code." << endl;
        return; 
    }

//CHANGED     cv::namedWindow(main_window_name,CV_WINDOW_NORMAL);
//CHANGED    cv::moveWindow(main_window_name, 400, 100);
//   cv::namedWindow(face_window_name,CV_WINDOW_NORMAL);
 //  cv::moveWindow(face_window_name, 10, 100);

// cv::namedWindow("Right Eye",CV_WINDOW_NORMAL);

// cv::moveWindow("Right Eye", 10, 600);
//CHANGED cv::namedWindow("Left Eye",CV_WINDOW_NORMAL);
//CHANGED cv::moveWindow("Left Eye", 10, 800);

  /* As the matrix dichotomy will not be applied, these windows are useless.
//CHANGED  cv::namedWindow("aa",CV_WINDOW_NORMAL);
//CHANGED  cv::moveWindow("aa", 10, 800);
//CHANGED  cv::namedWindow("aaa",CV_WINDOW_NORMAL);
//CHANGED  cv::moveWindow("aaa", 10, 800);*/

    createCornerKernels();
    ellipse(skinCrCbHist, cv::Point(113, 155), cv::Size(23, 15),
          43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1);

//Drawing goes here

  // I make an attempt at supporting both 2.x and 3.x OpenCV
    cv::VideoCapture capture(0);
  //  cout << capture.get(cv::CAP_PROP_BUFFERSIZE) << endl;

    capture.set(cv::CAP_PROP_BUFFERSIZE, 1);
    if(capture.isOpened()) 
    {
        while(true) 
        {
            capture.read(frame);

            // mirror it
            cv::flip(frame, frame, 1);
            frame.copyTo(debugImage);

            // Apply the classifier to the frame
            if(!frame.empty()) 
            {
                detectAndDisplay( frame );
            }
            else 
            {
                break;
            }
            imshow(main_window_name,debugImage);

	    std::this_thread::sleep_for(std::chrono::seconds(2));
//          std::cout << "Pupil Tracker thread sleeps" << endl;

#ifdef DEBUG
            int c = cv::waitKey(10);
            if ((char)c == 'c') 
            { 
                break;
            }
            if ((char)c == 'f') 
            {
                imwrite("frame.png",frame);
            }
#endif //DEBUG
        }
    }
    releaseCornerKernels();
    return;
}


void 
findEyes(cv::Mat frame_gray, cv::Rect face) 
{
    cv::Mat faceROI = frame_gray(face);
    cv::Mat debugFace = faceROI;

    if (kSmoothFaceImage) 
    {
        double sigma = kSmoothFaceFactor * face.width;
        GaussianBlur( faceROI, faceROI, cv::Size( 0, 0 ), sigma);
    }

  //-- Find eye regions and draw them
    int eye_region_width = face.width * (kEyePercentWidth/100.0);
    int eye_region_height = face.width * (kEyePercentHeight/100.0);
    int eye_region_top = face.height * (kEyePercentTop/100.0);

//  cv::Rect leftEyeRegion(face.width*(kEyePercentSide/100.0),
//                         eye_region_top,eye_region_width,eye_region_height);

    cv::Rect rightEyeRegion(face.width - eye_region_width - face.width*(kEyePercentSide/100.0),
                          eye_region_top,eye_region_width,eye_region_height);

//-- Find Eye Centers
//  cv::Point leftPupil = findEyeCenter(faceROI,leftEyeRegion,"Left Eye");
    cv::Point rightPupil = findEyeCenter(faceROI,rightEyeRegion,"Right Eye");

// get corner regions
//  cv::Rect leftRightCornerRegion(leftEyeRegion);
//  leftRightCornerRegion.width -= leftPupil.x;
//  leftRightCornerRegion.x += leftPupil.x;
//  leftRightCornerRegion.height /= 2;
//  leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
//  cv::Rect leftLeftCornerRegion(leftEyeRegion);
//  leftLeftCornerRegion.width = leftPupil.x;
//  leftLeftCornerRegion.height /= 2;
//  leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;

    cv::Rect rightLeftCornerRegion(rightEyeRegion);
    rightLeftCornerRegion.width = rightPupil.x;
    rightLeftCornerRegion.height /= 2;
    rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
    cv::Rect rightRightCornerRegion(rightEyeRegion);
    rightRightCornerRegion.width -= rightPupil.x;
    rightRightCornerRegion.x += rightPupil.x;
    rightRightCornerRegion.height /= 2;
    rightRightCornerRegion.y += rightRightCornerRegion.height / 2;

  //CHANGED - don't want left Eye -- rectangle(debugFace,leftRightCornerRegion,200);
  //CHANGED - don't want left Eye -- rectangle(debugFace,leftLeftCornerRegion,200);

    rectangle(debugFace,rightLeftCornerRegion,200);
    rectangle(debugFace,rightRightCornerRegion,200);

  // change eye centers to face coordinates
    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;

//  leftPupil.x += leftEyeRegion.x;
//  leftPupil.y += leftEyeRegion.y;

  // draw eye centers
    circle(debugFace, rightPupil, 3, 1234);

  //CHANGED - don't want leftPupil for now -- circle(debugFace, leftPupil, 3, 1234);
    cout << "RIGHT PUPIL: " << rightPupil.x << ", " << rightPupil.y << endl;

  //-- Find Eye Corners
    if (kEnableEyeCorner) 
    {
//    cv::Point2f leftRightCorner = findEyeCorner(faceROI(leftRightCornerRegion), true, false);
//    leftRightCorner.x += leftRightCornerRegion.x;
//    leftRightCorner.y += leftRightCornerRegion.y;
//    cv::Point2f leftLeftCorner = findEyeCorner(faceROI(leftLeftCornerRegion), true, true);
//    leftLeftCorner.x += leftLeftCornerRegion.x;
//    leftLeftCorner.y += leftLeftCornerRegion.y;

      cv::Point2f rightLeftCorner = findEyeCorner(faceROI(rightLeftCornerRegion), false, true);
      rightLeftCorner.x += rightLeftCornerRegion.x;
      rightLeftCorner.y += rightLeftCornerRegion.y;
      cv::Point2f rightRightCorner = findEyeCorner(faceROI(rightRightCornerRegion), false, false);
      rightRightCorner.x += rightRightCornerRegion.x;
      rightRightCorner.y += rightRightCornerRegion.y;

//    circle(faceROI, leftRightCorner, 3, 200);
//    circle(faceROI, leftLeftCorner, 3, 200);

      circle(faceROI, rightLeftCorner, 3, 200);
      circle(faceROI, rightRightCorner, 3, 200);
    }

//CHANGED -- imshow(face_window_name, faceROI);

//  cv::Rect roi( cv::Point( 0, 0 ), faceROI.size());
// cv::Mat destinationROI = debugImage( roi );
// faceROI.copyTo( destinationROI );
}


cv::Mat findSkin (cv::Mat &frame) {
  cv::Mat input;
  cv::Mat output = cv::Mat(frame.rows,frame.cols, CV_8U);

  cvtColor(frame, input, CV_BGR2YCrCb);

  for (int y = 0; y < input.rows; ++y) {
    const cv::Vec3b *Mr = input.ptr<cv::Vec3b>(y);
//    uchar *Or = output.ptr<uchar>(y);
    cv::Vec3b *Or = frame.ptr<cv::Vec3b>(y);
    for (int x = 0; x < input.cols; ++x) {
      cv::Vec3b ycrcb = Mr[x];
//      Or[x] = (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0) ? 255 : 0;
      if(skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) == 0) {
        Or[x] = cv::Vec3b(0,0,0);
      }
    }
  }
  return output;
}

/**
 * @function detectAndDisplay
 */
void 
detectAndDisplay(cv::Mat frame) 
{

    std::vector<cv::Rect> faces;
    //cv::Mat frame_gray;

    std::vector<cv::Mat> rgbChannels(3);
    cv::split(frame, rgbChannels);
    cv::Mat frame_gray = rgbChannels[2];

    //cvtColor( frame, frame_gray, CV_BGR2GRAY );
    //equalizeHist( frame_gray, frame_gray );
    //cv::pow(frame_gray, CV_64F, frame_gray);
    //-- Detect faces
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150));
//  findSkin(debugImage);

    for( int i = 0; i < faces.size(); i++ )
    {
        rectangle(debugImage, faces[i], 1234);
    }
    //-- Show what you got
    if (faces.size() > 0) 
    {
        findEyes(frame_gray, faces[0]);
    }
}

void 
myCircle(cv::Mat img, cv::Point center, int size, int B, int G, int R){
	cv::circle( img, center, size, cv::Scalar( B, G, R),cv::FILLED, cv::LINE_8 );
}

void 
dotMover(std::string msg)
{
    stdThreadSetPriority(20);

    bool results[40];
    cout << "Dot Mover Thread -- Begin" << msg << endl; 
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
//        std::cout << "Could not open or find"
//             << " the image";
  
        return;
    }
    int size = sizeof(x)/sizeof(x[0]);

    myCircle(image,cv::Point( 250, 250 ), 10, 0, 0, 255);
    imshow("window", image);
    cv::waitKey(1000);


    std::cout << "Value of size ..." << size << endl;
    for (int i=0; i < size; i++)
    {
        results[i] = false;
        time_t lBeginTime = time(0);
        time_t lNewTime  = time(0);
        while (lNewTime < (lBeginTime + 5000))
        {

            int c = cv::pollKey();
            if (c != -1)
            {
                results[i] = true;
                std::cout << "Input received - Key ..." << c << endl;
                break;
            }
	    std::this_thread::sleep_for(std::chrono::milliseconds(200));
            lNewTime = time(0);
        }
        myCircle(image,cv::Point(x[i],y[i]), 10, 170, 169, 173);
    	imshow("window", image);
    	myCircle(image,cv::Point(x[i], y[i]), 10, 255, 255 ,255);
    }
    cout << "Dot Mover Thread -- End" << msg << endl; 
    string name;
    cout << "Enter Results Filename" << endl;
    cin >> name;
    fstream fout;
    fout.open(name+ ".csv", ios::out | ios::app);
    for (int i=0; i<size; i++){
	fout << x[i] << "," << y[i] << "," << results[i] << "\n";
    }
    return;
}



