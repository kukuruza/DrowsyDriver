#include "histogram_generator_eyes.h"
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;



histogram_generator_eyes::histogram_generator_eyes(cluster_computation* clustering,feature_extractor* feature_extract)
{
    this->clustering=clustering;
    this->feature_extract=feature_extract;

    const string minPath ( string(CLASSIFIER_FACE_PATH) + "/Minimums_eyes" );
    const string rangePath ( string(CLASSIFIER_FACE_PATH) + "/Range_eyes" );

    if (! boost::filesystem::exists(minPath) || ! boost::filesystem::exists(rangePath) )
    {
        cerr << "Paths minPath or rangePath does not exist:" << endl
             << minPath << endl
             << rangePath << endl;
        throw exception();
    }

    cv::FileStorage fs(minPath.c_str(), cv::FileStorage::READ);
    fs["min_normalization"] >> min_normalization;

    cv::FileStorage fs2(rangePath.c_str(), cv::FileStorage::READ);
    fs2["range_normalization"] >> range_normalization;

}


void histogram_generator_eyes::compute_histogram(cv::Mat& image, cv::Mat& histogram){

    //Releasing the memory
    histogram.release();

    //Mat aux individual descriptor
    cv::Mat aux_descriptors;
    cv::Mat aux_histogram;
    cv::Mat aux_histogram2;

    //Aux info to compute the boxes
    int aux_cols;
    int aux_rows;
    //Boxes
    std::vector<box> boxes;

    //Cropped aux image
    cv::Mat cropped_image;

    //getting the size of the image
    aux_cols=image.cols;
    aux_rows=image.rows;

    //Getting the boxes

    std::vector<int> binx;
    std::vector<int> biny;
    binx.push_back(1);
    biny.push_back(1);
    boxes=generateSpatialBoxes(aux_rows, aux_cols, binx , biny);


    //Crop the images and extraction features
    for(unsigned int j=0;j<boxes.size();j++){

        cropped_image= image(cv::Range(boxes.at(j).minY, boxes.at(j).maxY), cv::Range(boxes.at(j).minX, boxes.at(j).maxX));
        aux_descriptors=feature_extract->extract_dsift(cropped_image,2,4);
        aux_histogram2=clustering->get_histogram(aux_descriptors);

        transpose(aux_histogram2, aux_histogram2);
        aux_histogram.push_back(aux_histogram2);

    }

    transpose(aux_histogram, aux_histogram);
    histogram.push_back(aux_histogram);
    aux_histogram.release();

}


std::vector<box> histogram_generator_eyes::generateSpatialBoxes(int rows, int cols, std::vector<int> binX, std::vector<int> binY){

    // rows -> binY
    // cols -> binX

    int current_binX;
    int current_binY;
    //For every layer we get the boxes
    std::vector<int> indexes_X;
    std::vector<int> indexes_Y;
    std::vector<box> output_boxes;

    box current_box;

    for(unsigned int i=0; i<binX.size();i++){

        //Getting the boxes
        current_binX=binX.at(i);
        current_binY=binY.at(i);
        //Indexes of the boxes...
        indexes_X=linspace(0, (cols-1), (current_binX+1));
        indexes_Y=linspace(0, (rows-1), (current_binY+1));

        for(unsigned int j=0; j<indexes_X.size()-1; j++){

            for(unsigned int k=0; k<indexes_Y.size()-1; k++){

                // Creating the boxes
                current_box.minX=indexes_X.at(j);
                current_box.maxX=indexes_X.at(j+1);
                current_box.minY=indexes_Y.at(k);
                current_box.maxY=indexes_Y.at(k+1);;

                // Storing the box
                output_boxes.push_back(current_box);

            }
        }
    }

    return output_boxes;
}




std::vector<int> histogram_generator_eyes::linspace(double a, double b, int n){

    std::vector<int> array;  // optional preallocation
    double step = (b-a) / (n-1);
    double current_index=0;
    current_index=a;

    while(current_index <= b) {
        array.push_back((int)current_index);
        current_index += step;           // could recode to better handle rounding errors
    }

    return array;
}



void histogram_generator_eyes::normalizing(cv::Mat& histogram, cv::Mat& histogram_norm)
{
    cv::Mat aux_row;
    aux_row= histogram - min_normalization;
    divide(histogram,range_normalization,aux_row);
    histogram_norm = aux_row;
}

