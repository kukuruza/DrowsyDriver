#ifndef KYLETRACKER_H
#define KYLETRACKER_H

#include <string>
#include <opencv2/core/core.hpp>
#include "Common.h"
#include "FaceTracker/Tracker.h"
#include "TrackerGeneric.h"
#include "KyleGeneric.h"


class KyleTracker : public TrackerGeneric, public KyleGeneric {
private:

    bool failed_kyle;

    // gray frame
    cv::Mat gray;

public:
    KyleTracker();

    // init - get model from KyleDetector
    bool init (const cv::Mat& _frame, const Face& _face, void* _anyData = NULL);
    bool processFrame (const cv::Mat& _frame, Face& _face);
};

#endif // KYLETRACKER_H
