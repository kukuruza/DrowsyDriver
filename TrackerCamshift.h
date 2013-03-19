#ifndef CAMSHIFTTRACKER_H
#define CAMSHIFTTRACKER_H

#include <opencv2/core/core.hpp>

class CAMShiftTracker
{
public:
    CAMShiftTracker();


private:

    // window
    cv::Rect box;
    cv::RotatedRect trackBox;

    cv::Mat roi;
    cv::Mat maskroi;
    cv::Mat mask_newFrame;

    cv::Mat hsv;
    cv::Mat hue;
    cv::Mat mask;
    cv::Mat hist;
    cv::Mat backproj;

    cv::Point origin;
    cv::Rect selection;

    int hmin;
    int hmax;

    int vmin;
    int vmax;

    int smin;

    int hsize;

    float hranges[2];

    //ranges in the array
    const float* phranges; //= hranges;


public:

    // give the first frame and seed the rectangle
    bool init(const cv::Mat& inputFrame, const cv::Rect& inputBox);


    // get new rectangle from new frame
    bool processFrame(const cv::Mat& newFrame, cv::Rect& resultBox);
};





// This wrapper class knows about the intristics of Face struct
//
#include "TrackerGeneric.h"
#include "Common.h"

class TrackerCamshiftFace : public TrackerGeneric {
private:

    CAMShiftTracker trackers[3];

public:

    bool init (const cv::Mat& _frame, const Face& _face, void* _anyData = NULL);
    bool processFrame (const cv::Mat& _frame, Face& _face);
};




#endif // CAMSHIFTTRACKER_H
