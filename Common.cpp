#include "Common.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


std::vector< cv::Rect* > Face::asPtrVector()
{
    std::vector< cv::Rect* > all;
    all.push_back(&face_rectangle);
        // the supported number of eyes is 0 - 2
    for (int i_eye = 0; i_eye != std::min(int(eye_rectangles.size()), 2); ++i_eye)
        all.push_back(&eye_rectangles[i_eye]);
    return all;
}
std::vector< const cv::Rect* > Face::asPtrVector() const
{
    std::vector< const cv::Rect* > all;
    all.push_back(&face_rectangle);
        // the supported number of eyes is 0 - 2
    for (int i_eye = 0; i_eye != std::min(int(eye_rectangles.size()), 2); ++i_eye)
        all.push_back(&eye_rectangles[i_eye]);
    return all;
}

bool Face::putOnImage(Mat& image, Scalar color) const
{
    try {
        std::vector< const cv::Rect* > facePtr = this->asPtrVector();
        for (unsigned int i = 0; i != facePtr.size(); ++i)
            rectangle(image, *facePtr[i], color);
        return 1;
    } catch(...) {
        cerr << "Face::putOnImage(): exception is caught." << endl;
        return 0;
    }
}

/*
bool Face::putOnImage(Mat& image, Scalar color, DetectorType type) const
{
    try {

        if (type==KYLE_DETECTOR)
        {

            int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;

            //draw triangulation
            c = CV_RGB(0,0,0);
            for(i = 0; i < tri.rows; i++){
                if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
                        visi.at<int>(tri.at<int>(i,1),0) == 0 ||
                        visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
                p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
                               shape.at<double>(tri.at<int>(i,0)+n,0));
                p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
                               shape.at<double>(tri.at<int>(i,1)+n,0));
                cv::line(image,p1,p2,c);
                p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
                               shape.at<double>(tri.at<int>(i,0)+n,0));
                p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
                               shape.at<double>(tri.at<int>(i,2)+n,0));
                cv::line(image,p1,p2,c);
                p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
                               shape.at<double>(tri.at<int>(i,2)+n,0));
                p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
                               shape.at<double>(tri.at<int>(i,1)+n,0));
                cv::line(image,p1,p2,c);
            }
            //draw connections
            c = CV_RGB(0,0,255);
            for(i = 0; i < con.cols; i++){
                if(visi.at<int>(con.at<int>(0,i),0) == 0 ||
                        visi.at<int>(con.at<int>(1,i),0) == 0)continue;
                p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),
                               shape.at<double>(con.at<int>(0,i)+n,0));
                p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),
                               shape.at<double>(con.at<int>(1,i)+n,0));
                cv::line(image,p1,p2,c,1);
            }
            //draw points
            for(i = 0; i < n; i++){
                if(visi.at<int>(i,0) == 0)continue;
                p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
                c = CV_RGB(255,0,0);
                cv::circle(image,p1,2,c);
            }
        }
        else
        {
            std::vector< const cv::Rect* > facePtr = this->asPtrVector();
            for (unsigned int i = 0; i != facePtr.size(); ++i)
                rectangle(image, *facePtr[i], color);
        }

        return 1;
    } catch(...) {
        cerr << "Error in Face.putOnImage()." << endl;
        return 0;
    }
}
*/

std::ostream& operator<< (std::ostream& os, const Face& face)
{
    std::vector< const cv::Rect* > facePtr = face.asPtrVector();
    for (unsigned int i = 0; i != facePtr.size(); ++i)
    {
        const cv::Rect* box = facePtr[i];
        os << (i == 0 ? "face: " : "      ")
           << "x:" << box->x << " y:" << box->y
           << " w:" << box->width << " h:" << box->height
           << (i == facePtr.size()-1 ? "" : "\n") << std::flush;
    }
    return os;
}


