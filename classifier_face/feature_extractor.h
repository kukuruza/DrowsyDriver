#ifndef FEATURE_EXTRACTOR_H
#define FEATURE_EXTRACTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vl/sift.h>
#include <vl/dsift.h>
#include <vector>
#include <algorithm>



class feature_extractor
{
public:

    feature_extractor();
    cv::Mat extract_dsift(cv::Mat& image,int step=2, int binSize=4);
};

#endif // FEATURE_EXTRACTOR_H
