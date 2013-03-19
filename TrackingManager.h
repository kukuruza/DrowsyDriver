#ifndef DrowsyDriver_Tracking_h
#define DrowsyDriver_Tracking_h

#include <iostream>
#include <exception>
#include <boost/thread/thread.hpp>
#include <boost/date_time.hpp>
#include <opencv2/core/core.hpp>

#include "Common.h"
#include "TrackerGeneric.h"
#include "TrackerCompressive.h"
#include "TrackerOpticalFlow.h"
#include "TrackerCamshift.h"
#include "TrackerKyle.h"



enum TrackerType { TRACKER_NOT_SET,
                   NOT_A_TRACKER,
                   COMPRESSIVE_TRACKER,
                   OPTICAL_FLOW_TRACKER,
                   CAMSHIFT_TRACKER,
                   KYLE_TRACKER
                 };


inline std::string writeTrackerType (TrackerType type)
{
    switch (type) {
        case TRACKER_NOT_SET      : return "tracker not set";
        case NOT_A_TRACKER        : return "not a tracker";
        case COMPRESSIVE_TRACKER  : return "compressive";
        case OPTICAL_FLOW_TRACKER : return "optical flow";
        case CAMSHIFT_TRACKER     : return "CAMshift";
        case KYLE_TRACKER         : return "kyle";
    } return "";
}


// TrackingMaster wrapper knows about both threads AND different types of trackers
//   Warning: it does not copy every frame. It is assumed to be run within a cycle
//
class TrackingMaster {
private:

      // tracker base class pointer
    TrackerType type;
    TrackerGeneric* tracker;

      // source frame and face
    cv::Mat image;
    Face face;

      // boost thread
    boost::thread th_thread;
      // status variable for monitoring the essentially void function work()
    bool th_worked_ok;
      // status variable to give to join in case never started
    bool th_running;
      // internal status to show that one real detector is allocated

    void allocTracker (TrackerType _type)
    {
        try {
            // clean previous tracker
            if (tracker != NULL)
                delete tracker;

            if (_type == COMPRESSIVE_TRACKER)
                tracker = new TrackerCompressiveFace;
            else if (_type == OPTICAL_FLOW_TRACKER)
                tracker = new TrackerOpticalFlowFace;
            else if (_type == CAMSHIFT_TRACKER)
                tracker = new TrackerCamshiftFace;
            else if (_type == KYLE_TRACKER)
                // warning: there is no check here that detector is Kyle detector
                tracker = new KyleTracker;
            else if (_type == NOT_A_TRACKER)
            {
                std::cerr << "TrackingMaster::allocTracker(): bad tracker type." << std::endl;
                throw std::exception();
            }
        } catch(...) {
            std::cerr << "Error in TrackingMaster::allocTracker()." << std::endl;
        }
    }

      //  image and anyData are essentially pointers, pass them at init()
    void work()
    {
        try {

            //std::cout << "tracker starteed " << std::endl;
            if (! image.data )
            {
                std::cerr << "TrackingMaster::work(): Bad image." << std::endl;
                th_worked_ok = 0;
                return;
            }
            if (tracker == NULL)
            {
                std::cerr << "TrackingMaster::work(): Tracker is not set (ptr == NULL)." << std::endl;
                th_worked_ok = 0;
                return;
            }

            th_worked_ok = tracker->processFrame(image, face);
            //std::cout << "th_worked_ok:" << th_worked_ok << std::endl;

        } catch(...) {
            std::cerr << "Error in TrackingMaster::work()." << std::endl;
            th_worked_ok = 0;
        }
    }

public:

    TrackingMaster (const TrackerType _type = TRACKER_NOT_SET) :
        type(_type), tracker(NULL), th_running(false)  { allocTracker(type); }

    bool changeType (const TrackerType _type)
    {
        try {
            if (type == _type)
            {
                //std::cerr << "Warning: TrackingMaster::changeType() - same type." << std::endl;
                return 1;
            }
            type = _type;
            allocTracker(type);
            return 1;
        } catch(...) {
            return 0;
        }
    }


      // ---------- init on every face redetection --------- //

      // note: no check for image validity on purpose - init may be done before first frame acquired
    bool init (const cv::Mat& _image, const Face& _face, void* _anyData = NULL)
    {
        try {

            face = _face;
            image = _image;

            return tracker->init(image, face, _anyData);

        } catch(...) {
            std::cerr << "Error in TrackerMaster.init()." << std::endl;
            return 0;
        }
    }
    

    // --------------- track on every frame --------------- //

      // interface without threads
    bool easyWork()    { work(); return th_worked_ok; }
    
      // interface with threads
    void start()       { th_running = true; th_thread = boost::thread(&TrackingMaster::work, this); }
    bool hasFinished() { return th_thread.timed_join(boost::posix_time::seconds(0)); }
    bool join()
    {
        try {
            if (th_thread.joinable()) th_thread.join();
            return th_worked_ok;
        } catch (...) {
            std::cerr << "TrackingMaster::join(): exception is caught."
                      << std::endl;
            return 0;
        }
    }




    // -------------------- results ----------------------- //

      // get results
    const Face& getFace() const { return face; }
    
};



#endif
