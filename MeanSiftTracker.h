//
//  MeanSiftTracker.h
//  DrowsyDriver
//
//  Created by Evgeny Toropov on 11/13/12.
//  Copyright (c) 2012 Evgeny Toropov. All rights reserved.
//

#ifndef DrowsyDriver_MeanSiftTracker_h
#define DrowsyDriver_MeanSiftTracker_h

#include <iostream>
#include <opencv2/core/core.hpp>

class MeanSiftTracker {
private:
    
    // window
    cv::Rect box;
    // reference histogram. Is not updated
    cv::MatND hist;
    // back projection
    cv::Mat backproj;

    // number of bins in histogram
    const int histSize;
    
    // Criteria to stop searching
    const cv::TermCriteria criteria;
    
public:
    
    MeanSiftTracker() :
        histSize (75),
        criteria (cv::TermCriteria(3, 20, 0.5)) { }
    

    
    // give the first frame and seed the rectangle
    bool init(const cv::Mat& inputFrame, const cv::Rect& inputBox);
    
    
    // get new rectangle from new frame
    bool processFrame(const cv::Mat& newFrame, cv::Rect& resultBox);
};


#endif
