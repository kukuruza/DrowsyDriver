#ifndef STRUCTS_H
#define STRUCTS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <algorithm>
#include <iostream>


#include <boost/thread/thread.hpp>
#include <boost/date_time.hpp>



struct Face {

    cv::Rect face_rectangle;
    std::vector <cv::Rect> eye_rectangles;
    
    // represent the face as a number of rectangles
    std::vector< cv::Rect* > asPtrVector();
    std::vector< const cv::Rect* > asPtrVector() const;
    
    // pointer to any model
    void* genericModel;

    //For kyle face detector visualization
    cv::Mat shape;
    cv::Mat con;
    cv::Mat tri;
    cv::Mat visi;

    // draw face on an image
    bool putOnImage(cv::Mat& image, cv::Scalar color = cv::Scalar(0,0,255)) const;
};

std::ostream& operator<< (std::ostream& os, const Face& face);



struct box {
    int minX, minY, maxX, maxY;
};



#endif // STRUCTS_H
