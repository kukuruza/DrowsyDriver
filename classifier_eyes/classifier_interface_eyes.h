#ifndef CLASSIFIER_INTERFACE_EYES_H
#define CLASSIFIER_INTERFACE_EYES_H

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <./classifier_eyes/svm_classifier_eyes.h>
#include <./classifier_face/cluster_computation.h>
#include <./classifier_face/feature_extractor.h>
#include <./classifier_eyes/histogram_generator_eyes.h>



enum EyeClosedType { BAD_EYE_CLASSIFIER = 0,
                     EYE_CLOSED = 1,
                     EYE_OPEN = -1
                   };

class ClassifierInterfaceEyes
{
public:

    // create new model
    ClassifierInterfaceEyes();
    // delete model
    ~ClassifierInterfaceEyes();

    EyeClosedType classify (const cv::Mat& image, const cv::Rect& faceRect );

private:

    histogram_generator_eyes* hist_generator;
    svm_classifier_eyes* classifier;
    cluster_computation* clustering;
    feature_extractor* feature_extract;
};


#endif // CLASSIFIER_INTERFACE_EYES_H
