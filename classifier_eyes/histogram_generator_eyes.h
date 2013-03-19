#ifndef HISTOGRAM_GENERATOR_EYES_H
#define HISTOGRAM_GENERATOR_EYES_H
#include <./classifier_face/cluster_computation.h>
#include <./classifier_face/feature_extractor.h>
#include <Common.h>


class histogram_generator_eyes
{

public:

    histogram_generator_eyes(cluster_computation* clustering,feature_extractor* feature_extract);
    void compute_histogram(cv::Mat& image, cv::Mat& histogram);
    void loadHists(int set);
    void normalizing(cv::Mat& histogram, cv::Mat& histogram_norm);

protected:

    cluster_computation* clustering;
    feature_extractor* feature_extract;

    std::vector<box> generateSpatialBoxes(int rows, int cols, std::vector<int> binX, std::vector<int> binY);
    std::vector<int> linspace(double a, double b, int n);

    cv::Mat min_normalization;
    cv::Mat range_normalization;

};
#endif // HISTOGRAM_GENERATOR_FACE_H
