#ifndef DrowsyDriver_OpticalFlowTracker_h
#define DrowsyDriver_OpticalFlowTracker_h

#include <opencv2/core/core.hpp>

// make a rectangle fit the image and validate its size
void validateRect (const cv::Size imsize, const int inner, const int border,
                   cv::Rect& rect);

    
class OpticalFlowTracker {
protected:
    
    // only a cropped part of a frame is tracked for speed.
    cv::Rect refBox;
    cv::Mat refFrameCrop;
    //
    // parameters for optical flow
    int maxLevel;
    double derivLambda;
    const int winsize;
    //
    // minimum box size. Used at initialization
    const int innerDepth;
    //
    // minumum border depth around box
    const int borderDepth;
    //
    // threshold for movement as percent of maximum
    const double thresMovePerc;
    
public:
    
    OpticalFlowTracker() :
        maxLevel    (3),
        derivLambda (0.5),
        winsize     (20),
        innerDepth  (10),
        borderDepth (winsize * 2),
        thresMovePerc (0.1)
        { }
    
    // give the first frame and seed the rectangle
    bool init(const cv::Mat& inputFrame, const cv::Rect& inputBox);
    
    // get new rectangle from new frame
    bool processFrame(const cv::Mat& newFrame, cv::Rect& resultBox);
};





// This wrapper class knows about the intristics of Face struct
//
#include "TrackerGeneric.h"
#include "Common.h"

class TrackerOpticalFlowFace : public TrackerGeneric {
private:

    OpticalFlowTracker trackers[3];

public:

    bool init (const cv::Mat& _frame, const Face& _face, void* _anyData = NULL);
    bool processFrame (const cv::Mat& _frame, Face& _face);
};




#endif
