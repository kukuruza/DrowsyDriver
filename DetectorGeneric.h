#ifndef DETECTORGENERIC_H
#define DETECTORGENERIC_H

#include <opencv2/core/core.hpp>
#include "Common.h"
#include <iostream>


// Base class for detectors
//
class DetectorGeneric {
public:

      // anyData passes whatever it needs to be passed (for Kyle)
    virtual inline bool init (void** _anyData = NULL)
    {
        (void)_anyData;
        return 1;
    }

      // performed within one cycle, no need to copy the frame
    virtual inline bool detect (const cv::Mat& _image, Face& face)
    {
        (void)_image;
        (void)face;
        return 1;
    }

};



#endif // DETECTORGENERIC_H
