#include "feature_extractor.h"

using namespace cv;
using namespace std;


feature_extractor::feature_extractor()
{
}


//Extracting DENSIFT for one image
Mat feature_extractor::extract_dsift(Mat& image,int step, int binSize){

    //Aux frame
    Mat currentFrame;
    //Mat descriptor
    Mat descriptor;

    //Double Magnif
    double magnif=6;
    double sigma;
    //Width and height
    int width;
    int height;

    //Pointer to the dsift_extractor
    VlDsiftFilter* dsift_extractor;
    //Pointer to the information of the keypoints
    VlDsiftKeypoint* keypoints;

    //Indexes for copying a descriptor to the descriptor
    //vector for training
    int index_upper_bound;
    int index_lower_bound;
    vector<float> current_descriptor;

    //Cast float because the vlfeat format
    image.convertTo(currentFrame, CV_32FC1);

    sigma= ((double)binSize)/magnif;
    //smoothing the image according to vlfeat phow descriptors
    GaussianBlur(currentFrame,currentFrame,Size(0,0),sigma,sigma,BORDER_DEFAULT);

    //Getting the dimensions
    width = currentFrame.cols;
    height = currentFrame.rows;

    //Creating the dsift descriptor
    dsift_extractor=vl_dsift_new_basic(width, height, step, binSize);

    //Processing the descriptors
    vl_dsift_process(dsift_extractor, (float*)currentFrame.data);

    //Getting the descriptors
    float* desc = dsift_extractor->descrs;

    //Getting the keypoints
    keypoints=dsift_extractor->frames;


    //Sampling for kmeans-> we have a probability of sampling a descriptor;

    for (int ii = 0; ii < dsift_extractor->numFrameAlloc; ++ ii) {

        index_lower_bound=ii*dsift_extractor->numBinAlloc;
        index_upper_bound=(ii+1)*dsift_extractor->numBinAlloc- 1;

        //Copying to a vector(auxiliary step)
        current_descriptor.insert(current_descriptor.begin(),&desc[index_lower_bound],&desc[index_lower_bound] + dsift_extractor->numBinAlloc);

        //Inserting the descriptor in the matrix
        Mat aux(current_descriptor,true);
        transpose(aux,aux);
        descriptor.push_back(aux);

        //Releasing memory
        aux.release();
        current_descriptor.clear();

    }

     vl_dsift_delete(dsift_extractor);

     return descriptor;
}





