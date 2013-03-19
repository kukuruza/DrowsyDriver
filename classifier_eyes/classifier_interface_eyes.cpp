#include "classifier_interface_eyes.h"
#include <boost/filesystem.hpp>
#include "evgUtilities/rectOperations.h"


using namespace std;
using namespace cv;


ClassifierInterfaceEyes::ClassifierInterfaceEyes()
{
    try {
        const string svmPath ( string(CLASSIFIER_FACE_PATH) + "/linear_svm_eyes.data" );
        const string vocabularyPath ( string(CLASSIFIER_FACE_PATH) + "/vocabulary_eyes.data" );

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

        classifier=new svm_classifier_eyes(svmPath);
        hist_generator=new histogram_generator_eyes(clustering,feature_extract);

    } catch(...) {
        cerr << "ClassifierInterfaceEyes() constructor error." << endl;
        throw exception();
    }
}


ClassifierInterfaceEyes::~ClassifierInterfaceEyes ()
{
    delete clustering;
    delete feature_extract;
    delete classifier;
    delete hist_generator;
}


EyeClosedType ClassifierInterfaceEyes::classify (const Mat& _image, const Rect& eyeRect)
{
    try {
        if (! _image.data || _image.type() != CV_8UC1)
        {
            cerr << "classifier_interface_face::classify(): bad image or image type." << endl;
            return BAD_EYE_CLASSIFIER;
        }

        const unsigned int minSize = 25;
        const double expandPercentage = 0.003;

        Rect expandedEyeRect = evg::expandRectPercImage (eyeRect, expandPercentage, _image.size(), minSize);

        Mat image_aux = _image(expandedEyeRect).clone();

        Mat histogram;
        Mat histogram_norm;

        int open_close;

        resize(image_aux,image_aux, Size(40,40));

        hist_generator->compute_histogram(image_aux,histogram);

        //Normalizing the data
        hist_generator->normalizing(histogram,histogram_norm);

        open_close=classifier->svm_classifier_classify(histogram_norm);

        return (EyeClosedType)open_close;
    } catch(...) {
        cerr << "ClassifierInterfaceEyes::classify(): exception." << endl;
        return BAD_EYE_CLASSIFIER;
    }
}


