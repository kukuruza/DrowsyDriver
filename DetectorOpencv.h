//
//  OpencvDetector.h
//  DrowsyDriver
//
//  Created by Francisco on 11/15/12.
//  Copyright (c) 2012 Francisco. All rights reserved.
//

#ifndef DrowsyDriver_OpencvDetector_h
#define DrowsyDriver_OpencvDetector_h

#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include "DetectorGeneric.h"
#include "Common.h"


class OpencvDetector : public DetectorGeneric {
private:

    std::string face_frontal_cascade_name;
    std::string face_profile_cascade_name;
    std::string eyes_cascade_name;

    cv::CascadeClassifier face_frontal_cascade;
    cv::CascadeClassifier face_profile_cascade;
    cv::CascadeClassifier eyes_cascade;
    
    // detector window constant
    int face_winSize;
    int eye_winSize;

public:
    
      // constructor also initializes path to data
    OpencvDetector();
    
      //
    bool detect(const cv::Mat& _image, Face& face);
    
};



#endif // DrowsyDriver_OpencvDetector
