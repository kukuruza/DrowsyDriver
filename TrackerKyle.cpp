#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/filesystem.hpp>
#include "evgUtilities/rectOperations.h"
#include "TrackerKyle.h"

using namespace std;
using namespace cv;




KyleTracker::KyleTracker()
{
    //failed_kyle = true;
}


bool KyleTracker::init (const cv::Mat& _frame, const Face& _face, void* modelData)
{
    try {

        (void)_frame;
        (void)_face;

        if (modelData == NULL)
        {
            cerr << "Error in KyleTracker::init() - modelData is NULL. "
                 << "KyleDetector::init() was probably skipped. "
                 << "You need to use KyleDetector with this tracker." << endl;
            return 0;
        }

        // Warning: careful with this line - it's typecast from void* !
        model = static_cast<FACETRACKER::Tracker *>(modelData);

        return 1;
    } catch(...) {
        cerr << "Error in KyleTracker::init()." << endl;
        return 0;
    }
}


bool KyleTracker::processFrame(const cv::Mat& newFrame, Face& face)
{
    try {

        if (model == NULL)
        {
            cerr << "KyleTracker::processFrame(): model == NULL." << endl;
            return 0;
        }

        if ( !newFrame.data || newFrame.type() != CV_8UC3)
        {
            cerr << "KyleTracker::processFrame(): bad new frame or frame type." << endl;
            return 0;
        }

        //Convert the image to gray
        cvtColor(newFrame, gray, CV_RGB2GRAY);


        //if failed change the size
        if (failed_kyle) wSize = wSize2;
        else wSize = wSize1;

        //Trying to track the face
        if (model->Track(gray,wSize,fpd,nIter,clamp,fTol,fcheck) == 0)
        {
            failed_kyle = false;
        }
        else
        {
            model->FrameReset();
            failed_kyle = true;
        }

        if (! processFace(newFrame.size(), face) )
            return 0;

        return 1;
    } catch(...) {
        cerr << "Error in KyleTracker::processFrame()." << endl;
        return 0;
    }
}

