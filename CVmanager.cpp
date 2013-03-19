#include <exception>
#include <boost/thread/thread.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "DetectingManager.h"
#include "TrackingManager.h"
#include "TriggerManager.h"
#include "Common.h"
#include "evgUtilities/mediaLoadSave.h"
#include "CVmanager.h"

using namespace cv;
using namespace std;

CVmanager::CVmanager()
:
  // output colors
  colorTrack (255,0,0),
  colorDetect (0,255,255),
  //
  frameRate (30),
  //
  defaultDetectorType (OPENCV_DETECTOR),
  defaultTrackerType (OPTICAL_FLOW_TRACKER),
  defaultInputType (evg::SrcVideo::CAMERA),
  minPeriodExp (0.),
  maxPeriodExp (4.),
  defaultPeriodExp (2.),
  //
  detectorType (defaultDetectorType),
  trackerType (defaultTrackerType)
  //
{
    setRedetectionPeriod(defaultPeriodExp);
}


bool CVmanager::setDetector()
{
    try{
        return detector.changeType(detectorType);
    } catch(...) {
        cerr << "Could not create detector of type number '" << detectorType << "'." << endl;
        return 0;
    }
}

bool CVmanager::setTracker()
{
    try{
        // Kyle tracker requires Kyle detector
        if (trackerType == KYLE_TRACKER && detectorType != KYLE_DETECTOR)
        {
            cerr << "Warning: Kyle tracker requires Kyle detector. "
                 << "Will change detector type to Kyle." << endl;
            detectorType = KYLE_DETECTOR;
            if (! setDetector() ) throw exception();
            if (! detector.init(&communicData) ) return 0;
        }
        //
        return tracker.changeType(trackerType);
    } catch(...) {
        cerr << "Could not create tracker of type number '" << trackerType << "'." << endl;
        return 0;
    }
}

bool CVmanager::setRedetectionPeriod(const double _periodExp)
{
    redetectionPeriodExp = _periodExp;
    redetectionPeriod = exp(_periodExp);
    return trigger.setParameter(redetectionPeriod);
}


bool CVmanager::start()
{
    try {
        // open resource
        if (! input.openResource (video) ) return 0;

        // put output video, if not blank
        if (outputPath != "")
            if (! evg::newVideo (outputPath, video, videoOutput)) return 0;

        // reset the recognition values
        integral_eyes = 0;
        integral_face = 0;

        // get and check a frame
        if ( !video.read(frame) || frame.type() != CV_8UC3)
        {
            cerr << "CVmanager(): Failed to read the first frame or bad frame type '"
                 << frame.type() << "', need '" << CV_8UC3 << "'." << endl;
            return 0;
        }

        // so that set() can be run
        everStarted = true;
        // should actually be init to false itself
        doFinishLoop = false;

        return 1;
    } catch(...) {
        cerr << "CVmanager::init(): exception is caught." << endl;
        return 0;
    }
}


bool CVmanager::prepareLoop()
{
    try {

        if (everStarted == false) return 1;

        communicData = NULL;

        // set detector and tracker (tracker must be the last)
        if (! setDetector() ) return 0;
        if (! setTracker() ) return 0;
        //
        if (trackerType == KYLE_TRACKER && detectorType != KYLE_DETECTOR)
        {
            cerr << "Kyle tracker requires Kyle detector." << endl;
            return 0;
        }

        if (! detector.init(&communicData) ) return 0;
        // Kyle tracker cannot be init without detector input
        detector.easyWork(frame, face);
        //
        if (! tracker.init(frame, face, communicData)) return 0;
        if (! trigger.init(frame) ) return 0;
        if (! recognizer.reset() ) return 0;

        detector_working = false;
        recognizer_working = false;

        everPreparedLoop = true;

        return 1;
    } catch(...) {
        cerr << "CVmanager::prepareLoop(): exception is caught." << endl;
        return 0;
    }
}


bool CVmanager::runLoop()
{
    try {
        if (everStarted == false) return 1;
        if (everPreparedLoop == false) return 1;

        th_worked_ok = false;
        th_thread = boost::thread(&CVmanager::th_workLoop, this);
        return 1;
    } catch(...) {
        cerr << "CVmanager::runLoop(): exception caught" << endl;
        return 0;
    }
}


bool CVmanager::stopLoop()
{
    try {
        doFinishLoop = true;
        th_thread.join();
        doFinishLoop = false;
        return th_worked_ok;
    } catch(...) {
        cerr << "CVmanager::stopLoop(): exception caught" << endl;
        return 0;
    }
}


bool CVmanager::finish()
{
    try {
        everStarted = false;

        face = Face();

        return input.closeResource(video);
        // no close for output video VideoWriter. It reinits at open()
    } catch(...) {
        cerr << "CVmanager::finish(): exception caught." << endl;
        return 0;
    }
}

void CVmanager::th_workLoop()
{
    try {

        // note: the cycle frequency is limited by tracker and trigger, not frame rate
        while (1)
        {
            timer.start((unsigned int)(1000 / frameRate));

            // stop if finished video
            if ( !video.read(frame) )
            {
                detector.join();
                th_worked_ok = true;
                break;
            }
            // check new frame
            if ( !frame.data || frame.type() != CV_8UC3)
            {
                cerr << "CVmanager(): Failed to read a frame or bad frame type  '"
                     << frame.type() << "', need '" << CV_8UC3 << "'." << endl;
                return;
            }

            // if the detector has just finished
            if ( detector_working && detector.hasFinished() )
            {
                detector.join();
                face = detector.getFace();
                if (! tracker.init(frame, face, communicData) ) return;
                detector_working = false;
            }

            // if it is time to run detector
            if ( !detector_working && trigger.verdict() )
            {
    //            cout << "detection" << endl;
                if (! trigger.init(frame) ) return;
                detector.setFrame(frame);
                detector.start();
                detector_working = true;
            }

            // if the recognizer has just finished
            if ( recognizer_working && recognizer.hasFinished() )
            {
                recognizer.join();
                recognizer_working = false;
                raw_output_face_classifier=recognizer.getCurrentFaceState();
                raw_output_eye_classifier=recognizer.getCurrentEyesState();
                integral_face=recognizer.getIntegralFaceState();
                integral_eyes=recognizer.getIntegralEyesState();
            }

            // if it is time to run recognizer
            if ( !recognizer_working )
            {
     //           cout << "recognize" << endl;
                recognizer.setFrame(frame);
                recognizer.start(face);
                recognizer_working = true;
            }

     //       cout << "track" << endl;
            // there is so much trouble with Kyletracker
            if ( trackerType == KYLE_TRACKER &&
                 (detectorType != KYLE_DETECTOR || detector_working == 1) )
                ;
            else
                tracker.start();
            //
            trigger.start();
            if (! trigger.join() ) return;
            if (! tracker.join() )
            {
                cerr << "Tracker is joined with an error" << endl;
                return;
            }

            face = tracker.getFace();
            //cout << face << endl;

            // lock access to showFrame and update it
            {
                boost::unique_lock<boost::mutex> lock(frame_mutex);

                showFrame = frame.clone();
                Scalar color = (detector_working ? colorDetect : colorTrack);
                face.putOnImage(showFrame, color);
                drawResult();
            }
            frame_notifier.notify_all();

            //
            if (outputPath != "")
                videoOutput.write(showFrame);

            timer.wait();

            if (doFinishLoop)
                break;
        }
    } catch (...) {
        cerr << "CVmanager::th_workLoop(): exception is caught." << endl;
        return;
    }

    cout << "CVmanager::th_workLoop() exited the loop." << endl;

    try {
        timer.wait();
        if (! detector.join() || ! recognizer.join() )
            cerr << "CVmanager::th_workLoop(): cannot join detector and recognizer." << endl;
        else
            th_worked_ok = true;

    } catch (...) {
        cerr << "CVmanager::th_workLoop(): exception during clean up." << endl;
    }
}



// draw everything
void CVmanager::drawResult()
{
    try {

        const unsigned int baseSize = frame.size().height;
        const Scalar goodColor (0,255,0), badColor (0,0,255);
        //
        // base sizes. All variable parameters are set here
        const Point2f detectorTextPerc (0.05, 0.07);
        const Point2f trackerTextPerc (0.05, 0.14);
        const Point2f faceTextPerc (0.50, 0.875);
        const Point2f eyeTextPerc (0.50, 0.965);
        const double circleRadPerc = 0.03;
        const Point2f faceCircPerc (0.07, 0.865);
        const Point2f eyeCircPerc (0.07, 0.955);
        const Point2f face1perc (0.13, 0.85), face2perc (0.45, 0.88);
        const Point2f eye1perc (0.13, 0.94), eye2perc (0.45, 0.97);
        //
        // write text
        Point2d faceText (faceTextPerc.x * baseSize, faceTextPerc.y * baseSize);
        Point2d eyeText  (eyeTextPerc.x * baseSize, eyeTextPerc.y * baseSize);
        Point2d detectorText (detectorTextPerc.x * baseSize, detectorTextPerc.y * baseSize);
        Point2d trackerText  (trackerTextPerc.x * baseSize, trackerTextPerc.y * baseSize);
        double fontScale = 0.0012 * baseSize;
        int thickness = 3 * fontScale;
        putText(showFrame, "looking on/off the road", faceText, FONT_HERSHEY_SIMPLEX,
                fontScale, goodColor, thickness);
        putText(showFrame, "eyes are open/closed", eyeText, FONT_HERSHEY_SIMPLEX,
                fontScale, goodColor, thickness);
        putText(showFrame, writeDetectorType(detectorType) + " detector", detectorText,
                FONT_HERSHEY_SIMPLEX, fontScale, goodColor, thickness);
        putText(showFrame, writeTrackerType(trackerType) + " tracker", trackerText,
                FONT_HERSHEY_SIMPLEX, fontScale, goodColor, thickness);
        //
        // draw circles
        Point2d faceCenter(faceCircPerc.x * baseSize, faceCircPerc.y * baseSize);
        Point2d eyeCenter(eyeCircPerc.x * baseSize, eyeCircPerc.y * baseSize);
        unsigned int circleRad = circleRadPerc * baseSize;
        Scalar faceColor = (raw_output_face_classifier == FACE_ON_THE_ROAD ? goodColor : badColor);
        int eyesThickness = (raw_output_face_classifier == FACE_ON_THE_ROAD ? CV_FILLED : 1);
        Scalar eyeColor = (raw_output_eye_classifier == EYE_OPEN ? goodColor : badColor);
        circle(showFrame, faceCenter, circleRad, faceColor, CV_FILLED);
        circle(showFrame, eyeCenter, circleRad, eyeColor, eyesThickness);
        //
        // draw face rectangle
        double faceMidPerc = face1perc.x + integral_face * (face2perc.x - face1perc.x);
        unsigned int faceRight = face1perc.x * baseSize;
        unsigned int faceTop = face1perc.y * baseSize;
        unsigned int faceLeft = face2perc.x * baseSize;
        unsigned int faceBottom = face2perc.y * baseSize;
        unsigned int faceMid = faceMidPerc * baseSize;
        rectangle(showFrame, Point2d(faceRight, faceTop), Point2d(faceMid, faceBottom),
                  badColor, CV_FILLED);
        rectangle(showFrame, Point2d(faceMid, faceTop), Point2d(faceLeft, faceBottom),
                  goodColor, CV_FILLED);
        //
        // draw eye rectangle
        double eyeMidPerc = eye1perc.x + integral_eyes * (eye2perc.x - eye1perc.x);
        unsigned int eyeRight = eye1perc.x * baseSize;
        unsigned int eyeTop = eye1perc.y * baseSize;
        unsigned int eyeLeft = eye2perc.x * baseSize;
        unsigned int eyeBottom = eye2perc.y * baseSize;
        unsigned int eyeMid = eyeMidPerc * baseSize;
        rectangle(showFrame, Point2d(eyeRight, eyeTop), Point2d(eyeMid, eyeBottom),
                  badColor, eyesThickness);
        rectangle(showFrame, Point2d(eyeMid, eyeTop), Point2d(eyeLeft, eyeBottom),
                  goodColor, eyesThickness);

    } catch (...) {
        cerr << "CVmanager::drawResult(): exception is caught" << endl;
    }
}



