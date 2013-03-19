#include "svm_classifier_eyes.h"

svm_classifier_eyes::svm_classifier_eyes(std::string path_loading_svm)
{
    svm_classifier_load(path_loading_svm);
}

int svm_classifier_eyes::svm_classifier_classify(cv::Mat& histogram)
{
    int result;
    result=(int)svm_instance.predict(histogram);
    //std::cout<<result<<std::endl;
    return result;
}

void svm_classifier_eyes::svm_classifier_load(std::string path_loading_svm)
{
    //Saving the classifier
    svm_instance.load(path_loading_svm.c_str());
}

