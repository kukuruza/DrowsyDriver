#ifndef CVMANAGER_H
#define CVMANAGER_H

#include <iostream>
#include <exception>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/thread/thread.hpp>

#include "DetectingManager.h"
#include "TrackingManager.h"
#include "TriggerManager.h"
#include "Recognizer.h"
#include "Common.h"
#include "evgUtilities/timers.h"
#include "evgUtilities/mediaLoadSave.h"


class CVmanager {
private:

    // boost thread
    boost::thread th_thread;
    // status variable for monitoring the essentially void function work()
    bool th_worked_ok;
    // loop function
    void th_workLoop();
    // trigger for the loop to stop
    bool doFinishLoop;

    bool setDetector();
    bool setTracker();

    // draw everything on showFrame
    void drawResult();


    cv::VideoCapture    video;
    cv::VideoWriter     videoOutput;
    cv::Mat             frame;

    Face                face;
    DetectingMaster     detector;
    TrackingMaster      tracker;
    TriggerMaster       trigger;
    Recognizer          recognizer;
    evg::Timer          timer;

    double              redetectionPeriod;
    double              redetectionPeriodExp;

    cv::Scalar          colorTrack, colorDetect;

    bool                detector_working;
    bool                recognizer_working;

    //communication data is used to send model* from Kyle detector to Kyle tracker
    void*               communicData;

    const unsigned int  frameRate;

    // prevent prepareLoop() and runLoop() in case never started
    bool everStarted;
    bool everPreparedLoop;

    FaceOnOffRoadType raw_output_face_classifier;
    EyeClosedType raw_output_eye_classifier;
    float integral_face;
    float integral_eyes;


public:

      // high-level //

    CVmanager ();
    bool start();        // init the input and output video
    bool prepareLoop();  // init the tracker and detector
    bool runLoop();      // go
    bool stopLoop();     // stop to reinit tracker and detector
    bool finish();       // stop to reinit input and output video


      // parameters //

      // taken by gui for reference
    const DetectorType        defaultDetectorType;
    const TrackerType         defaultTrackerType;
    const evg::SrcVideo::Type defaultInputType;
    const double              minPeriodExp;
    const double              maxPeriodExp;
    const double              defaultPeriodExp;

      // set by gui and taken by gui back for confirmation (may change)
    DetectorType            detectorType;
    TrackerType             trackerType;

      // set by gui before start()
    evg::SrcVideo           input;
    std::string             outputPath;

      // set and get by gui on-the-fly
    bool setRedetectionPeriod (const double periodExp);
    inline double getRedetectionPeriodExp () const { return redetectionPeriodExp; }


      // pass result frame to a gui //

    cv::Mat showFrame;
      // mutex
    boost::mutex frame_mutex;
      // notification
    boost::condition_variable frame_notifier;

};


#endif // CVMANAGER_H
