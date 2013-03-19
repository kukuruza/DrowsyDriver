//
//  MeanSiftTracker.cpp
//  DrowsyDriver
//
//  Created by Evgeny Toropov on 11/13/12.
//  Copyright (c) 2012 Evgeny Toropov. All rights reserved.
//

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include "MeanSiftTracker.h"

using namespace std;
using namespace cv;

bool MeanSiftTracker::init(const cv::Mat& inputFrame, const cv::Rect& inputBox)
{
try {
    
    // Ranges of gray. Do NOT change
    const float range_one[2] = { 0, 256 };
    const float* ranges = { range_one };
    
    // initialize the box
    box = inputBox;
    
    // crop input frame
    cv::Mat inputFrameCrop = inputFrame(box).clone();
    
    // calculate histogram for the cropped frame    
    calcHist( &inputFrameCrop, 1, 0, cv::Mat(), hist, 1,
                 &histSize, &ranges, true, false );
    cv::normalize( hist, hist, 0, 255, cv::NORM_MINMAX, -1, cv::Mat() );
    
    return 1;
    
} catch(...) {
    std::cerr << "Error in MeanSiftTracker::init()." << std::endl;
    return 0;
}
}

bool MeanSiftTracker::processFrame(const cv::Mat& newFrame, cv::Rect& resultBox)
{
try {
    // Ranges of gray. Do NOT change
    const float range_one[2] = { 0, 256 };
    const float* ranges = { range_one };
    
    cv::calcBackProject( &newFrame, 1, 0, hist, backproj, &ranges, 1, true );
    cv::meanShift( backproj, box, criteria );
    
    resultBox = box;
    return 1;
} catch(...) {
    std::cerr << "Error in MeanSiftTracker::processFrame()." << std::endl;
    return 0;
}
}

