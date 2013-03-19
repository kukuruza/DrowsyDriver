#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>

#include "TrackerCamshift.h"

using namespace std;
using namespace cv;


CAMShiftTracker::CAMShiftTracker() :
    hmin (0),
    hmax (180),
    vmin (25),
    vmax (256),
    smin (30),
    hsize (45)
{
    hranges[0]=hmin;
    hranges[1]=hmax;
    phranges=hranges;
}


bool CAMShiftTracker::init(const cv::Mat& inputFrame, const cv::Rect& inputBox)
{
    try {


        // initialize the box
        box = inputBox;

        //Converting the input frame to HSV
        cvtColor(inputFrame, hsv, CV_BGR2HSV);

        //Mask with the values inside of the range for hsv
        //Since we have 3 dimensions on the hsv, for every pixel
        //we have to give 3 values to compare
        inRange(hsv, cv::Scalar(hmin, smin, MIN(vmin,vmax)), cv::Scalar(hmax, 256, MAX(vmin, vmax)), mask);

        int ch[] = {0, 0};

        //Geting the hue component of the image
        //Creating the image
        hue.create(hsv.size(), hsv.depth());
        //Coppying the channel hue of hsv to hue
        mixChannels(&hsv, 1, &hue, 1, ch, 1);

        //Getting the hue values of the template
        roi= hue(inputBox);
        //Getting the mask values of the template
        maskroi=mask(inputBox);

        //Computing the color histogram
        //Get the histogram for roi, 1 mat only, channel 0, mask to compute only on those pixels
        //output value, dimension of the histogram, and the ranges [hmin hmax]
        calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);

        //Normalizing the histogram
        normalize(hist, hist, 0, 255, CV_MINMAX);



        return 1;

    } catch(...) {
        std::cerr << "Error in MeanSiftTracker::init()." << std::endl;
        return 0;
    }


}

bool CAMShiftTracker::processFrame(const cv::Mat& newFrame, cv::Rect& resultBox)
{
    try {

        Mat hsv_newframe;
        Mat hue_newframe;

        //Converting the new image to HSV format
        cvtColor(newFrame, hsv_newframe, CV_BGR2HSV);

        //Getting the mask
        inRange(hsv_newframe, cv::Scalar(hmin, smin, MIN(vmin,vmax)), cv::Scalar(hmax, 256, MAX(vmin, vmax)), mask_newFrame);

        //Coppying the channel hue of hsv to hue
        hue_newframe.create(hsv_newframe.size(), hsv_newframe.depth());
        int ch[] = {0, 0};
        mixChannels(&hsv_newframe, 1, &hue_newframe, 1, ch, 1);

        //Getting backproject
        calcBackProject(&hue_newframe,1,0,hist,backproj,&phranges);
        backproj &= mask_newFrame;
        RotatedRect trackBox = CamShift(backproj, box, TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ));


        resultBox=trackBox.boundingRect();

        //cout<<trackBox.center.x<<"  "<<trackBox.center.y <<endl;

        //Mat image;
        //cvtColor( backproj, image, CV_GRAY2BGR );

        //imshow("back projection",image);
        //waitKey(10);

        //box=trackBox.boundingRect();

        return 1;

    } catch(...) {
        std::cerr << "Error in MeanSiftTracker::processFrame()." << std::endl;
        return 0;
    }

}





bool TrackerCamshiftFace::init (const Mat& _frame, const Face& _face, void* _anyData)
{
    try {

        (void)_anyData;

        // get rectangles from face
        const std::vector< const cv::Rect* > face_parts = _face.asPtrVector();
        if ( face_parts.size() > 3 )
        {
            cerr << "Error in TrackerCamshiftFace::init() "
                 << "- more than 3 face rectangles." << endl;
            return 0;
        }

        // init every rectangles
        for (unsigned int part = 0; part != face_parts.size(); ++part )
            if (! trackers[part].init(_frame, *face_parts[part]) ) return 0;

        return 1;
    } catch(...) {
        cerr << "Exception in TrackerCamshiftFace::init()." << endl;
        return 0;
    }
}

bool TrackerCamshiftFace::processFrame (const Mat& _frame, Face& _face)
{
    try {

        // get rectangles from face
        std::vector< cv::Rect* > face_parts = _face.asPtrVector();
        if ( face_parts.size() > 3 )
        {
            cerr << "Error in TrackerCamshiftFace::processFrame() "
                 << "- more than 3 face rectangles." << endl;
            return 0;
        }

        // process every rectangles
        for (unsigned int part = 0; part != face_parts.size(); ++part )
            if (! trackers[part].processFrame(_frame, *face_parts[part]) ) return 0;

        return 1;
    } catch(...) {
        cerr << "Exception in TrackerCamshiftFace::processFrame()." << endl;
        return 0;
    }
}


