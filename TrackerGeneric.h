#ifndef TRACKERGENERIC_H
#define TRACKERGENERIC_H

#include <opencv2/core/core.hpp>
#include "Common.h"


// Base class for trackers
//
class TrackerGeneric {
public:

    // initialize everything, anyData passes whatever it needs to be passed (for Kyle)
    virtual inline bool init (const cv::Mat& _frame, const Face& _face, void* _anyData = NULL)
    {
        (void)_frame;
        (void)_face;
        (void)_anyData;
        return 1;
    }

    // performed within one cycle, no need to copy the frame
    virtual inline bool processFrame (const cv::Mat& _frame, Face& _face)
    {
        (void)_frame;
        (void)_face;
        return 1;
    }

};

#endif // TRACKERGENERIC_H
