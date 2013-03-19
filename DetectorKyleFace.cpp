#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "DetectorKyleFace.h"

using namespace std;
using namespace cv;


bool KyleFaceDetector::detect(const cv::Mat& _image, Face& face)
{
    try {

        if ( !_image.data || _image.type() != CV_8UC3)
        {
            cerr << "KyleFaceDetector::detect(): bad new frame or frame type." << endl;
            return 0;
        }

        //Converting the image to gray
        cvtColor (_image, gray, CV_RGB2GRAY);


        model->FrameReset();

        wSize = wSize2;
        if (model->Track(gray, wSize, fpd, nIter, clamp, fTol, fcheck) == 1)
            ; // nothing so far

        // write face parameters
        if (! processFace(_image.size(), face) ) return 0;

        return 1;
    } catch(...) {
        cerr << "Error in KyleDetector::detect()." << endl;
        return 0;
    }
}
