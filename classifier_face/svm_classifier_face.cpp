#include "svm_classifier_face.h"

using namespace cv;
using namespace std;


svm_classifier_face::svm_classifier_face(string path_loading_svm)
{
    svm_classifier_load(path_loading_svm);
}

int svm_classifier_face::svm_classifier_classify(Mat& histogram)
{
    int result;
    result=(int)svm_instance.predict(histogram);
    //cout<<result<<endl;
    return result;
}

void svm_classifier_face::svm_classifier_load(string path_loading_svm)
{
    //Saving the classifier
    svm_instance.load(path_loading_svm.c_str());
}

