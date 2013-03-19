#include "classifier_interface_face.h"
#include <boost/filesystem.hpp>
#include "evgUtilities/rectOperations.h"

using namespace cv;
using namespace std;


ClassifierInterfaceFace::ClassifierInterfaceFace ()
{
    try {
        const string svmPath ( string(CLASSIFIER_FACE_PATH) + "/linear_svm_onOff.data" );
        const string vocabularyPath ( string(CLASSIFIER_FACE_PATH) + "/vocabulary_onOff.data" );

        if (! boost::filesystem::exists(svmPath) ||
                ! boost::filesystem::exists(vocabularyPath) )
        {
            cerr << "Paths svmPath or vocabularyPath does not exist:" << endl
                 << svmPath << endl
                 << vocabularyPath << endl;
            throw exception();
        }

        clustering=new cluster_computation(vocabularyPath);
        feature_extract=new feature_extractor();

        classifier=new svm_classifier_face(svmPath);
        hist_generator=new histogram_generator_face(clustering,feature_extract);

    } catch(...) {
        cerr << "ClassifierInterfaceFace() constructor error." << endl;
        throw exception();
    }
}


ClassifierInterfaceFace::~ClassifierInterfaceFace ()
{
    delete clustering;
    delete feature_extract;
    delete classifier;
    delete hist_generator;
}


FaceOnOffRoadType ClassifierInterfaceFace::classify(const Mat& image, const Rect& faceRect)
{
    try {
        if (! image.data || image.type() != CV_8UC1)
        {
            cerr << "classifier_interface_face::classify(): bad image or image type." << endl;
            return BAD_FACE_CLASSIFIER;
        }

        const unsigned int minSize = 30;
        const double expandPercentage = 0.03;

        Rect expandedFaceRect = evg::expandRectPercImage (faceRect, expandPercentage, image.size(), minSize);

        vector<float> points;
        points.push_back(expandedFaceRect.x);
        points.push_back(expandedFaceRect.y);
        points.push_back(expandedFaceRect.x + expandedFaceRect.width);
        points.push_back(expandedFaceRect.y);
        points.push_back(expandedFaceRect.x + expandedFaceRect.width);
        points.push_back(expandedFaceRect.y + expandedFaceRect.height);
        points.push_back(expandedFaceRect.x);
        points.push_back(expandedFaceRect.y + expandedFaceRect.height);

        Mat histogram;
        Mat histogram_norm;

        int onOff_road;
        Point p1;
        Point p2;
        Point p3;
        Point p4;

        p1.x=points.at(0);
        p1.y=points.at(1);

        p2.x=points.at(2);
        p2.y=points.at(3);

        p3.x=points.at(4);
        p3.y=points.at(5);

        p4.x=points.at(6);
        p4.y=points.at(7);

        Mat image_aux = image(expandedFaceRect).clone();
        resize (image_aux, image_aux, Size(200,200));

        hist_generator->compute_histogram(image_aux, histogram, points);
        //Normalizing the data
        hist_generator->normalizing(histogram, histogram_norm);

        onOff_road = classifier->svm_classifier_classify(histogram_norm);

        return (FaceOnOffRoadType)onOff_road;
    } catch(...) {
        cerr << "ClassifierInterfaceFace::classify(): exception." << endl;
        return BAD_FACE_CLASSIFIER;
    }
}


Mat ClassifierInterfaceFace::extractFace(Mat image, Point p1, Point p2, Point p3, Point p4, double percentage){

    (void)p3;

    double height = sqrt(pow((double)(p4.x-p1.x),2) + pow((double)(p4.y-p1.y),2));
    double width = sqrt(pow((double)(p2.x-p1.x),2) + pow((double)(p2.y-p1.y),2));

    double newHeight = height + height*(percentage/100);
    double newWidth = width + width*(percentage/100);

    int minX = cv::min(p1.x,p4.x);
    int minY = cv::min(p1.y,p2.y);

    //Make a rectangle
    Rect roi(minX - (newWidth-(width))/2, minY - (newHeight-(height))/2,newWidth,newHeight);
    //Point a cv::Mat header at it (no allocation is done)
    Mat croppedImage = image(roi);

    return croppedImage;
}

