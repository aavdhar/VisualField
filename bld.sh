#!/bin/bash
export OCLIBS=$OCBUILD/lib
echo "Building findEye"
g++ -g -std=c++11 \
        -I$OC/include/opencv2 \
        -I$OC/modules/objdetect/include \
        -I$OC/modules/core/include \
        -I$OCBUILD \
        -I$OC/modules/highgui/include \
        -I$OC/modules/imgcodecs/include \
        -I$OC/modules/videoio/include \
        -I$OC/modules/imgproc/include \
        -I$OC/modules/flann/include \
        -I$OC/modules/features2d/include \
        -o findEye \
        findEyeCenter.cpp \
        findEyeCorner.cpp \
        main.cpp \
        helpers.cpp \
        -pthread \
        -L$OCLIBS \
        -lopencv_highgui -lopencv_objdetect -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc -lopencv_core -lpthread

#echo "Building DrawOCV"
#g++ -g -std=c++11 \
#        -I$OC/include/opencv2 \
#        -I$OC/modules/objdetect/include \
#        -I$OC/modules/core/include \
#        -I$OCBUILD \
#        -I$OC/modules/highgui/include \
#        -I$OC/modules/imgcodecs/include \
##        -I$OC/modules/videoio/include \
#        -I$OC/modules/imgproc/include \
#        -I$OC/modules/flann/include \
#        -I$OC/modules/features2d/include \
#        -o drawOcv \
#        drawocv.cpp \
#        -L$OCLIBS \
#        -lopencv_highgui -lopencv_objdetect -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc -lopencv_core

