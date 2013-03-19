#ifndef HISTOGRAM_GENERATOR_H
#define HISTOGRAM_GENERATOR_H

#include "cluster_computation.h"
#include "feature_extractor.h"
#include "Common.h"

class histogram_generator_face
{

public:

    histogram_generator_face(cluster_computation* clustering, feature_extractor* feature_extract);
    void compute_histogram(cv::Mat& image, cv::Mat& histogram, std::vector<float> points);
    void loadHists(int set);
    void normalizing(cv::Mat& histogram, cv::Mat& histogram_norm);

protected:

    cluster_computation* clustering;
    feature_extractor* feature_extract;

    std::vector<box> generateSpatialBoxes(int rows, int cols, std::vector<int> binX, std::vector<int> binY);
    void normalize_points(std::vector<float>& points);
    std::vector<int> linspace(double a, double b, int n);

    cv::Mat min_normalization;
    cv::Mat range_normalization;

};

#endif // HISTOGRAM_GENERATOR_H
