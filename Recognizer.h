#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <string>
#include <iostream>
#include <exception>
#include <boost/thread/thread.hpp>
#include <boost/date_time.hpp>
#include <opencv2/core/core.hpp>
#include "Common.h"
#include "classifier_face/classifier_interface_face.h"
#include "classifier_eyes/classifier_interface_eyes.h"


class Recognizer {
private:

    ClassifierInterfaceFace recogFace;
    ClassifierInterfaceEyes recogEyes;

    boost::thread th_threadFace, th_threadEyes;
    bool th_working;

    // copy of frame
    cv::Mat grayImage;
    // contains face and eyes rectangles
    Face face;

    // results:
    //
    // recognition state at the current run
    FaceOnOffRoadType currentFaceState;
    EyeClosedType currentEyeState;
    // accumulates currentStates over several frames
    float integralFaceState;
    float integralEyesState;
    // inertia shows how hard a currentState affects an integralState
    const float faceInertia;
    const float eyeInertia;

    // these functions are made separate for:
    //   1) using thread or non-thread interface, 2) processing exceptions within a thread
    void workFace();
    void workEyes();


public:

    // set inertia, and reset the current states to N/A
    Recognizer() : faceInertia(0.333), eyeInertia(0.333) { reset(); }

    // not used now
    inline bool init() { return 1; }

    // reset the current states to N/A
    inline bool reset()
    {
        currentFaceState = BAD_FACE_CLASSIFIER;
        currentEyeState = BAD_EYE_CLASSIFIER;
        return 1;
    }

    // copy the frame so that the class can go into a separate thread
    bool setFrame (const cv::Mat& image);

    // non-thread interface
    inline bool easyWork (const Face& _face)
    {
        try {
            face = _face;
            workFace();
            workEyes();
            return 1;
        } catch(...) {
            std::cerr << "Recognizer::easyWork() exception is caught" << std::endl;
            return 0;
        }
    }

    // thread interface
    inline bool start (const Face& _face)
    {
        try {
            th_working = true;
            face = _face;
            th_threadFace = boost::thread( &Recognizer::workFace, this );
            th_threadEyes = boost::thread( &Recognizer::workEyes, this );
            return 1;
        } catch(...) {
            std::cerr << "Recognizer::start() exception is caught" << std::endl;
            return 0;
        }
    }
    inline bool join()
    {
        try {
            //if (!th_working) return 1;
            if (th_threadFace.joinable()) th_threadFace.join();
            if (th_threadEyes.joinable()) th_threadEyes.join();
            //th_working = false;
            return 1;
        } catch (boost::thread_interrupted&) {
            // in this case ignore what we are told. Need to finish
            if (th_threadFace.joinable()) th_threadFace.join();
            if (th_threadEyes.joinable()) th_threadEyes.join();
            // give interruption back to the function that called evgTimer
            throw boost::thread_interrupted();
        } catch(...) {
            std::cerr << "Recognizer::join() exception is caught" << std::endl;
            return 0;
        }
    }
    inline bool hasFinished()
    {
        try {
            return th_threadFace.timed_join(boost::posix_time::seconds(0));
        } catch(...) {
            std::cerr << "Recognizer::hasFinished() exception is caught" << std::endl;
            return 0;
        }
    }

    // gets
    inline FaceOnOffRoadType getCurrentFaceState() const { return currentFaceState; }
    inline EyeClosedType     getCurrentEyesState() const { return currentEyeState; }
    inline float getIntegralFaceState() const { return integralFaceState; }
    inline float getIntegralEyesState() const { return integralEyesState; }

};


#endif // RECOGNIZER_H
