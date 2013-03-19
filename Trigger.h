#ifndef __DrowsyDriver_1__Trigger__
#define __DrowsyDriver_1__Trigger__

#include <iostream>
#include <opencv2/core/core.hpp>


// class responsible for triggering face detection
//   verdict() function gives the answer if redetection is ok
//
//   Redetection is triggered if the motion on the frame is minimum since init.
//   Besides, there is 1) a minimum time 2) penalty for long time
//
class Trigger {
private:

    // assumpton is that main manages ref- and newFrame
    cv::Mat refFrameSmall;
    cv::Mat newFrameSmall;
    cv::Mat refFrameSmallGray;
    
    // scaling of all input frames
    const double scaleFactor;
    // time since reinit
    long time;
    
    // current normalized result
    double resultNormalized;
    
    // minimum time before firing
    const unsigned int minVerdictTime;
    // limit for resultNormalized, when redetection is triggered
    const double normalizedLimit;
    // average redetection time
    double redetectPeriod;

public:

    Trigger() :
        scaleFactor     (0.25),
        minVerdictTime  (10),
        normalizedLimit (.01),
        redetectPeriod  (4)
        { }

    // set longTimePenalty
    bool setRedetectPeriod (const double _penalty);

    // call after every detection
    bool setFrame (const cv::Mat& _refFrame);
    
    // feed new frame
    bool processFrame (const cv::Mat& _newFrame);
    
    // see if redetection is ok
    //   (function is exceptions-safe)
    bool verdict() const;

};


#endif
