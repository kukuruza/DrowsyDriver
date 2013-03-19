#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include "histogram_generator_face.h"

using namespace cv;
using namespace std;

histogram_generator_face::histogram_generator_face(cluster_computation* clustering,feature_extractor* feature_extract)
{
    this->clustering=clustering;
    this->feature_extract=feature_extract;

    const string minPath ( string(CLASSIFIER_FACE_PATH) + "/Minimums_onOff" );
    const string rangePath ( string(CLASSIFIER_FACE_PATH) + "/Range_onOff" );

    if (! boost::filesystem::exists(minPath) ||
        ! boost::filesystem::exists(rangePath) )
    {
        cerr << "Paths minPath or rangePath does not exist:" << endl
             << minPath << endl
             << rangePath << endl;
        throw exception();
    }

    FileStorage fs(minPath.c_str(), FileStorage::READ);
    fs["min_normalization"] >> min_normalization;

    FileStorage fs2(rangePath.c_str(), FileStorage::READ);
    fs2["range_normalization"] >> range_normalization;

}


void histogram_generator_face::compute_histogram(Mat& image, Mat& histogram, vector<float> points){

    //Releasing the memory
    histogram.release();

    //Mat aux individual descriptor
    Mat aux_descriptors;
    Mat aux_histogram;
    Mat aux_histogram2;

    //Aux info to compute the boxes
    int aux_cols;
    int aux_rows;
    //Boxes
    vector<box> boxes;

    //Cropped aux image
    Mat cropped_image;

    //getting the size of the image
    aux_cols=image.cols;
    aux_rows=image.rows;

    //Getting the boxes

    vector<int> binx;
    vector<int> biny;
    binx.push_back(3);
    biny.push_back(3);
    boxes=generateSpatialBoxes(aux_rows, aux_cols, binx , biny);



    //Crop the images and extraction features
    for(unsigned int j=0;j<boxes.size();j++){

        cropped_image= image(cv::Range(boxes.at(j).minY, boxes.at(j).maxY), cv::Range(boxes.at(j).minX, boxes.at(j).maxX));
        aux_descriptors=feature_extract->extract_dsift(cropped_image,4,4);
        aux_histogram2=clustering->get_histogram(aux_descriptors);

        transpose(aux_histogram2, aux_histogram2);
        aux_histogram.push_back(aux_histogram2);

    }


    //Adding the boxes!!
    normalize_points(points);
    Mat aux(points);
    aux_histogram.push_back(aux);

    transpose(aux_histogram, aux_histogram);
    histogram.push_back(aux_histogram);
    aux_histogram.release();

}


vector<box> histogram_generator_face::generateSpatialBoxes(int rows, int cols, vector<int> binX, vector<int> binY){

    // rows -> binY
    // cols -> binX

    int current_binX;
    int current_binY;
    //For every layer we get the boxes
    vector<int> indexes_X;
    vector<int> indexes_Y;
    vector<box> output_boxes;

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

void histogram_generator_face::normalize_points(vector<float>& points){

    float x_normalizer=1280;
    float y_normalizer=720;
    float aux_value;

    float rem;

    for(int i=0;i<8;i++){
        aux_value=points.at(i);
        rem=i%2;

        if(rem==0)
            points.at(i)=(aux_value/x_normalizer);
        else
            points.at(i)=(aux_value/y_normalizer);
    }



}


vector<int> histogram_generator_face::linspace(double a, double b, int n){

    vector<int> array;  // optional preallocation
    double step = (b-a) / (n-1);
    double current_index=0;
    current_index=a;

    while(current_index <= b) {
        array.push_back((int)current_index);
        current_index += step;           // could recode to better handle rounding errors
    }

    return array;
}



void histogram_generator_face::normalizing(Mat& histogram, Mat& histogram_norm)
{
    Mat aux_row;
    //cout<<"size of the histogram "<<histogram.rows<<" "<<histogram.cols<<endl;
    //cout<<"size of the range "<<range_normalization.rows<<" "<<range_normalization.cols<<endl;


    aux_row= histogram - min_normalization;
    divide(histogram,range_normalization,aux_row);
    histogram_norm = aux_row;
}

