#ifndef __TriggerManager__
#define __TriggerManager__

#include <boost/thread/thread.hpp>
#include <boost/date_time.hpp>
#include <opencv2/core/core.hpp>
#include "Trigger.h"


// TriggerMaster is a wrapper for Trigger class using boost::thread
//
class TriggerMaster {
private:
    // thread
    boost::thread th_thread;
    // status variable for monitoring essentially void function work()
    bool worked_ok;
    
    Trigger trigger;
    cv::Mat frame;

    void work()
    {
        worked_ok = trigger.processFrame(frame);
    }

public:
    TriggerMaster() { }
    
    // reinit after detection
    bool init (const cv::Mat& _refFrame)
    {
        frame = _refFrame;
        return trigger.setFrame(frame);
    }

    // set trigger parameter
    bool setParameter (const double _param)
    {
        return trigger.setRedetectPeriod (_param);
    }


    // run without threads
    bool easyRun ()
    {
        work();
        return worked_ok;
    }
    
    // start thread
    void start() { th_thread = boost::thread(&TriggerMaster::work, this); }
    
    // check if thread complete
    bool hasFinished() { return th_thread.timed_join(boost::posix_time::seconds(0)); }
    
    // finish thread (wait for completion if necessary)
    bool join()
    {
        try {
            if (th_thread.joinable()) th_thread.join();
            return worked_ok;
        } catch (...) {
            std::cerr << "TriggerMaster::join(): exception is caught."
                      << std::endl;
            return 0;
        }
    }

    // get verdict for redetection
    bool verdict()
    {
        return trigger.verdict();
    }
    

};

#endif
