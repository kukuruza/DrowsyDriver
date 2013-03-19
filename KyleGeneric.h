#ifndef KYLEGENERIC_H
#define KYLEGENERIC_H

#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>

#include "Common.h"
#include "DetectorGeneric.h"
#include "FaceTracker/include/FaceTracker/Tracker.h"


class KyleGeneric {

public:
    KyleGeneric();
    ~KyleGeneric();

protected:

    //Kyle detector parameters
    FACETRACKER::Tracker* model;
    bool failed_kyle;

    // for face drawing
    cv::Mat tri;
    cv::Mat con;

    // model parameters
    std::vector<int> wSize;
    std::vector<int> wSize1;
    std::vector<int> wSize2;
    const int nIter;
    const double clamp;
    const double fTol;
    const bool fcheck;
    const int fpd;

    // eye_rectangles sides will be shifted as a percent of rectangle size
    std::vector<double> expandEyesPerc;
    std::vector<double> expandFacePerc;

    bool processFace (const cv::Size& _imageSize, Face& face);
};

#endif // KYLEGENERIC_H
