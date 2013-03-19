#ifndef DrowsyDriver_Detecting_h
#define DrowsyDriver_Detecting_h

#include <vector>
#include <iostream>
#include <exception>
#include <boost/thread/thread.hpp>
#include <boost/date_time.hpp>
#include <opencv2/core/core.hpp>

#include "DetectorGeneric.h"
#include "DetectorOpencv.h"
#include <DetectorKyleFace.h>



enum DetectorType { DETECTOR_NOT_SET=0,
                    NOT_A_DETECTOR=1,
                    OPENCV_DETECTOR=2,
                    KYLE_DETECTOR=3
                  };


inline std::string writeDetectorType (DetectorType type)
{
    switch (type) {
        case DETECTOR_NOT_SET : return "detector not set";
        case NOT_A_DETECTOR   : return "not a detector";
        case OPENCV_DETECTOR  : return "opencv";
        case KYLE_DETECTOR    : return "kyle";
    } return "";
}


// DetectorMaster class is a wrapper that knows about both threads AND different detectors
// all technique functions take an RGB image and a reference to face on input
//
class DetectingMaster {
private:

      // boost thread
    boost::thread th_thread;
      // status variable for monitoring essentially void function work()
    bool th_worked_ok;

      // COPY of input
    cv::Mat image;
      // src and result
    Face face;

    // different types of detectors
    DetectorType type;
    DetectorGeneric* detector;
    

    void allocDetector (DetectorType _type)
    {
          // clean previous detector
        if (detector != NULL)
            delete detector;

        if (_type == OPENCV_DETECTOR)
            detector = new OpencvDetector;
        else if (_type == KYLE_DETECTOR)
            detector = new KyleFaceDetector;
        else if (_type == NOT_A_DETECTOR)
        {
            std::cerr << "DetectingMaster::allocDetector(): bad detector type." << std::endl;
            throw std::exception();
        }
    }

    void work()
    {
        try {

            if (! image.data )
            {
                std::cerr << "DetectingMaster::work(): Bad image." << std::endl;
                th_worked_ok = 0;
                return;
            }
            if (detector == NULL)
            {
                std::cerr << "DetectingMaster::work(): Detector is not set (ptr == NULL)."
                          << std::endl;
                th_worked_ok = 0;
                return;
            }

            th_worked_ok = detector->detect(image, face);

        } catch(...) {
            std::cerr << "Error in DetectingMaster::work()." << std::endl;
            th_worked_ok = 0;
        }
    }

    
public:

    DetectingMaster (const DetectorType _type = DETECTOR_NOT_SET) :
        type(_type), detector(NULL) { allocDetector(type); }

    bool changeType (const DetectorType _type)
    {
        try {
            if (type == _type)
            {
                //std::cerr << "Warning: DetectingMaster::changeType() - same type." << std::endl;
                return 1;
            }
            type = _type;
            allocDetector(type);
            return 1;
        } catch(...) {
            return 0;
        }
    }


    // ---------- init in the beginning after creation or change type --------- //

      // note: no check for imgae validity on purpose - init may be done before first frame acquired
    bool init (void** _anyData = NULL)
    {
        return detector->init(_anyData);
    }


    // --------------- track on every frame --------------- //

      // interface without threads
    bool easyWork(const cv::Mat& _image, Face& _face)
    {
        try {
            image = _image.clone();
            work();
            _face = face;
            return th_worked_ok;
        } catch(...) {
            std::cerr << "Error in DetectingMaster::easyWork()." << std::endl;
            return 0;
        }
    }

    // interface with threads
    // init: save a copy of frame
    void setFrame (const cv::Mat& _image)
                       { image = _image.clone(); }
    void start()       { th_thread = boost::thread(&DetectingMaster::work, this); }
    bool hasFinished() { return th_thread.timed_join(boost::posix_time::seconds(0)); }
    bool join()
    {
        try {
            if (th_thread.joinable()) th_thread.join();
            return th_worked_ok;
        } catch (...) {
            std::cerr << "DetectingMaster::join(): exception is caught."
                      << std::endl;
            return 0;
        }
    }


    // -------------------- results ----------------------- //

      // get results
    const Face& getFace() const { return face; }

};




#endif
