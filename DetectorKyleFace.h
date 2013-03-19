#ifndef KYLEFACEDETECTOR_H
#define KYLEFACEDETECTOR_H

#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>
#include "Common.h"
#include "DetectorGeneric.h"
#include "KyleGeneric.h"
#include "FaceTracker/Tracker.h"


class KyleFaceDetector : public DetectorGeneric, public KyleGeneric {
public:

    bool init (void** KyleModelPtr)
    {
        *KyleModelPtr = static_cast<void *>(model);
        return 1;
    }

    bool detect(const cv::Mat& _image, Face& face);

private:

    bool failed_kyle;

    // frame
    cv::Mat gray;

};

#endif // KYLEFACEDETECTOR_H
