#include "TrackerOpticalFlow.h"

#include <iostream>
#include <vector>
#include <exception>
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include "evgUtilities/rectOperations.h"

using namespace std;
using namespace cv;



bool OpticalFlowTracker::init(const cv::Mat& inputFrame, const cv::Rect& inputBox)
{
    try {

        refBox = inputBox;
        
        // correct the position of inputBox within inputFrame, if necessary
        evg::validateRect(inputFrame.size(), innerDepth, borderDepth, refBox);
        
        // increase the rectangle we use
        cv::Rect refBoxExpanded = evg::expandRect(refBox, borderDepth);
        
        // crop the input frame up to refBoxExpanded and seed it to class
        refFrameCrop = inputFrame(refBoxExpanded).clone();
        //if (refFrameCrop.type() == CV_8UC3)
        //    cvtColor( refFrameCrop, refFrameCrop, CV_RGB2GRAY );
        
        return 1;
    } catch(...) {
        std::cout << "Error in OpticalFlowTracker::init()." << std::endl;
        throw;
    }
}

bool OpticalFlowTracker::processFrame(const cv::Mat& newFrame, cv::Rect& resultBox)
{
    try {
        // increase the rectangle we need to tackle
        cv::Rect refBoxExpanded = evg::expandRect(refBox, borderDepth);
        
        // crop new frame
        cv::Mat newFrameCrop = newFrame(refBoxExpanded).clone();
        
        // get good points for optical flow tracking
        vector<Point2f> refPoints, newPoints;
        const int maxPoints = 50;
        // mask border around expanded rectangle
        Mat maskPoints = Mat::zeros(refFrameCrop.size(), CV_8UC1);
        Rect innerRect (Point(borderDepth, borderDepth), refBox.size());
        rectangle(maskPoints, innerRect, Scalar(255), CV_FILLED);
        // process color
        cv::Mat refFrameCropGray = refFrameCrop;
        if (refFrameCrop.type() == CV_8UC3)
        {
            refFrameCropGray = refFrameCropGray.clone();
            cvtColor( refFrameCrop, refFrameCropGray, CV_RGB2GRAY );
        }
        // find points
        goodFeaturesToTrack(refFrameCropGray, refPoints, maxPoints, 0.01, 4, maskPoints);
        
        // if no points found - no motion
        Scalar delta;
        if (refPoints.size() != 0)
        {
        
            // optical flow
            vector<uchar> pointStatus;
            vector<float> pointErr;
            Size winsizeSq = Size(winsize, winsize);
            calcOpticalFlowPyrLK(refFrameCrop, newFrameCrop, refPoints, newPoints,
                                 pointStatus, pointErr, winsizeSq, maxLevel,
                                 TermCriteria( TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01),
                                 derivLambda);
            

            // getting the mean
            vector<Point2f> deltaVect (refPoints.size());
            for (unsigned long i = 0; i != refPoints.size(); ++i)
                if (pointStatus[i])
                    deltaVect[i] = newPoints[i] - refPoints[i];

            // filtering non-moving points
            vector<double> absVect (deltaVect.size());
            double maxAbsVect = 0;
            for (unsigned int i = 0; i != deltaVect.size(); ++i)
            {
                // get flow norm
                absVect[i] = norm(deltaVect[i]);
                // update max flow norm
                if (absVect[i] > maxAbsVect)
                    maxAbsVect = absVect[i];
            }
            for (long i = absVect.size()-1; i != -1; --i)
                // for every small flow norm
                if (absVect[i] < thresMovePerc * maxAbsVect)
                {
                    // erase element
                    deltaVect.erase(deltaVect.begin() + i);
                    absVect.erase(absVect.begin() + i);
                }

            delta = cv::mean(deltaVect);
        }
        int dx = round(double(delta[0]));
        int dy = round(double(delta[1]));
        
        // write the result
        resultBox.x = refBox.x + dx;
        resultBox.y = refBox.y + dy;
        
        // validate result
        evg::validateRect(newFrame.size(), innerDepth, borderDepth, resultBox);
        
        // update the reference box
        cv::Rect resultBoxExpanded = evg::expandRect(resultBox, borderDepth);
        refFrameCrop = newFrame(resultBoxExpanded).clone();
        //if (refFrameCrop.type() == CV_8UC3)
        //    cvtColor( refFrameCrop, refFrameCrop, CV_RGB2GRAY );
        refBox = resultBox;
        
        return 1;
    } catch(...) {
        std::cout << "Error in OpticalFlowTracker::processFrame()." << std::endl;
        throw;
    }
}








bool TrackerOpticalFlowFace::init (const Mat& _frame, const Face& _face, void* _anyData)
{
    try {

        (void)_anyData;

        // get rectangles from face
        const std::vector< const cv::Rect* > face_parts = _face.asPtrVector();
        if ( face_parts.size() > 3 )
        {
            cerr << "Error in TrackerOpticalFlowFace::init() "
                 << "- more than 3 face rectangles." << endl;
            return 0;
        }

        // init every rectangles
        for (unsigned int part = 0; part != face_parts.size(); ++part )
            if (! trackers[part].init(_frame, *face_parts[part]) ) return 0;

        return 1;
    } catch(...) {
        cerr << "Exception in TrackerOpticalFlowFace::init()." << endl;
        return 0;
    }
}

bool TrackerOpticalFlowFace::processFrame (const Mat& _frame, Face& _face)
{
    try {

        // get rectangles from face
        std::vector< cv::Rect* > face_parts = _face.asPtrVector();
        if ( face_parts.size() > 3 )
        {
            cerr << "Error in TrackerOpticalFlowFace::processFrame() "
                 << "- more than 3 face rectangles." << endl;
            return 0;
        }

        // process every rectangles
        for (unsigned int part = 0; part != face_parts.size(); ++part )
            if (! trackers[part].processFrame(_frame, *face_parts[part]) ) return 0;

        return 1;
    } catch(...) {
        cerr << "Exception in TrackerOpticalFlowFace::processFrame()." << endl;
        return 0;
    }
}


