#include <iostream>
#include "Trigger.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

using namespace std;
using namespace cv;


bool Trigger::setRedetectPeriod(const double _redetectPeriod)
{
    redetectPeriod = _redetectPeriod;
    return 1;
}

bool Trigger::setFrame (const cv::Mat& _refFrame)
{
    try {
    
        // reset time
        time = 0;
        
        // resize input frame
        resize (_refFrame, refFrameSmall, Size(),
                scaleFactor, scaleFactor, INTER_AREA);
        
        return 1;
    } catch(...) {
        cerr << "Error in Trigger::reinit()." << endl;
        return 0;
    }
}


bool Trigger::processFrame (const cv::Mat& _newFrame)
{
    try {
    
        // resize input frame
        resize (_newFrame, newFrameSmall, Size(),
                scaleFactor, scaleFactor, INTER_AREA);
        
        // get grayscale version
        refFrameSmallGray = refFrameSmall;
        if (refFrameSmall.type() == CV_8UC3)
        {
            refFrameSmallGray = refFrameSmallGray.clone();
            cvtColor( refFrameSmall, refFrameSmallGray, CV_RGB2GRAY );
        }
        
        // get good points for optical flow tracking
        vector<Point2f> refPoints, newPoints;

        // minimum distance between points as percent of frame size
        Size imSize = refFrameSmall.size();
        const double minPointDistPerc = 0.1;
        unsigned int minPointDist = round( minPointDistPerc * (imSize.width+imSize.height)/2 );
        // find good points
        const unsigned int maxPoints = 50;
        goodFeaturesToTrack(refFrameSmallGray, refPoints, maxPoints, 0.01, minPointDist);

        // if no points found - no motion
        double delta = numeric_limits<double>::max();
        if (refPoints.size() != 0)
        {
            // optical flow
            const int maxLevel = 3;
            const double derivLambda = 0.5;
            vector<uchar> pointStatus;
            vector<float> pointErr;
            // optical flow search window size
            const int winsize = 20;
            Size winsizeSq = Size(winsize, winsize);
            calcOpticalFlowPyrLK (refFrameSmall, newFrameSmall, refPoints, newPoints,
                                  pointStatus, pointErr, winsizeSq, maxLevel,
                                  TermCriteria( TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01),
                                  derivLambda);
            
            // compute delta
            vector<Point2f> deltaVect (refPoints.size());
            for (unsigned long i = 0; i != refPoints.size(); ++i)
                if (pointStatus[i])
                    deltaVect[i] = newPoints[i] - refPoints[i];
            
            // compute delta norm
            vector<double> absVect (deltaVect.size());
            for (unsigned long i = 0; i != deltaVect.size(); ++i)
                absVect[i] = norm(deltaVect[i]);

            // threshold static non-face objects
            const double deltaThreshold = 0.2;
            for (int i = (int)absVect.size()-1; i != -1; --i)
                if (absVect[i] < deltaThreshold)
                {
                    deltaVect.erase(deltaVect.begin() + i);
                    absVect.erase(absVect.begin() + i);
                }
            
            // get the mean
            if (absVect.size() != 0)
                delta = cv::mean(absVect)[0];
        }
        
        // update normalized results
        if (time == 0)
            resultNormalized = numeric_limits<double>::max();
        else
            resultNormalized = delta / exp(double(time) * (1 / redetectPeriod));
        
        // cout << "normalized: " << resultNormalized << endl;
        
        // renew reference frame
        refFrameSmall = newFrameSmall.clone();
        
        // implement time
        ++time;
        
        return 1;
    } catch(...) {
        cerr << "Error in Trigger::setFrame()." << endl;
        return 0;
    }
}


bool Trigger::verdict() const
{
    // if normalized minimum is reached
    if (resultNormalized < normalizedLimit)
        return 1;
    
    return 0;
}

