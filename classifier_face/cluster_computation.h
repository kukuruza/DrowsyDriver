#ifndef CLUSTER_COMPUTATION_H
#define CLUSTER_COMPUTATION_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/flann/flann.hpp>
#include <opencv2/flann/kdtree_index.h>
#include <opencv2/flann/kmeans_index.h>
#include <opencv2/flann/autotuned_index.h>


class cluster_computation
{

public:

    cluster_computation(std::string vocabulary);
    cv::Mat get_histogram(cv::Mat& descriptors);
    cv::Mat generateHistogram( cv::Mat& indexes_for_descriptor);
    void loadCodeWords_search(std::string& _fileName);
    int getNumber_clusters();

protected:

    //Loading and saving data
    void saveCodeWords(std::string& _fileName);
    void loadCodeWords(std::string& _fileName);

    //Centers
    cv::Mat centers;

    //Number of clusters
    int number_clusters;
    //End criteria for kmeans
    cv::TermCriteria term_criteria;

    //Indexes for the knn search
    cv::flann::Index flann_indexes;
    //Automatic parameters for the knn search
    cv::flann::AutotunedIndexParams auto_params;


};

#endif // CLUSTER_COMPUTATION_H
