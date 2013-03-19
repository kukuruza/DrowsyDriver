#ifndef CLASSIFIER_INTERFACE_FACE_H
#define CLASSIFIER_INTERFACE_FACE_H



#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <classifier_face/svm_classifier_face.h>
#include <classifier_face/cluster_computation.h>
#include <classifier_face/feature_extractor.h>
#include <classifier_face/histogram_generator_face.h>



enum FaceOnOffRoadType { BAD_FACE_CLASSIFIER = 0,
                         FACE_ON_THE_ROAD = -1,
                         FACE_RIGHT = 1,
                         FACE_LEFT = 2
                       };

class ClassifierInterfaceFace {
public:

    // create new model
    ClassifierInterfaceFace();
    // delete model
    ~ClassifierInterfaceFace();

    FaceOnOffRoadType classify (const cv::Mat& image, const cv::Rect& faceRect);


private:

    cv::Mat extractFace(cv::Mat image, cv::Point p1, cv::Point p2, cv::Point p3, cv::Point p4,
                        double percentage = 10.00);

    histogram_generator_face* hist_generator;
    svm_classifier_face* classifier;
    cluster_computation* clustering;
    feature_extractor* feature_extract;

};

#endif // CLASSIFIER_INTERFACE_H
