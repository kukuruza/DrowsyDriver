#include "cluster_computation.h"

using namespace cv;
using namespace std;


cluster_computation::cluster_computation(string vocabulary)
{
    loadCodeWords_search(vocabulary);
}



//Get a single histogram
Mat cluster_computation::get_histogram(Mat& descriptors)
{

    //KNN search to find nearest cluster for each descriptor
    Mat index_descriptor;
    Mat output_distances;
    Mat histogram;

    //I want only the nearest, hard clustering
    int knn=1;
    //The number of times the tree(s) in the index should be recursively traversed. A
    //higher value for this parameter would give better search precision, but also take more time.
    int numberOfChecks=32;

    //Indexes for each descriptor
    flann_indexes.knnSearch(descriptors,index_descriptor,output_distances,knn,cv::flann::SearchParams(numberOfChecks));

    //getting the histograms
    histogram=generateHistogram(index_descriptor);

    return histogram;
}


void cluster_computation::loadCodeWords_search(string& _fileName)
{

    //Loading the centers
    loadCodeWords(_fileName);
    cout<<_fileName<<endl;
    number_clusters=centers.rows;

    /*Getting ready for the knn search*/
    float target_precision = 0.9;
    float build_weight = 0.01;
    float memory_weight = 0;
    float sample_fraction = 0.1;

    //Automatic parameters for the knn search
    //auto_params(target_precision,build_weight,memory_weight,sample_fraction);
    auto_params.setFloat("target_precision",target_precision);
    auto_params.setFloat("build_weight",build_weight);
    auto_params.setFloat("memory_weight",memory_weight);
    auto_params.setFloat("sample_fraction",sample_fraction);
    //Indexes for the knn search
    flann_indexes.build(centers,auto_params);
}



void cluster_computation::loadCodeWords(std::string& _fileName)
{
    Mat aux;
    cv::FileStorage fs(_fileName.c_str(), FileStorage::READ);
    fs["codeWords"] >> aux;
    printf("Loaded Mat of size %d x %d from file '%s'\n", aux.rows, aux.cols, _fileName.c_str());
    centers=aux.clone();
}



//Generates a single normed histogram over given cluster memberships
Mat cluster_computation::generateHistogram(Mat& indexes_for_descriptor)
{

    //Histogram(one column)
    Mat histogram = cv::Mat::zeros(number_clusters, 1, CV_32F);
    Mat normedHistogram = cv::Mat::zeros(number_clusters, 1, CV_32F);

    int accumulator = 0;

    for (; accumulator != indexes_for_descriptor.rows; ++accumulator) {
        ++histogram.at<float>(indexes_for_descriptor.at<int>(accumulator, 0), 0);
    }


    for (int i = 0; i != histogram.rows; ++i) {
        normedHistogram.at<float>(i,0) = histogram.at<float>(i,0) / (float)accumulator;
    }

    //It returns row
    return normedHistogram.t();

}
