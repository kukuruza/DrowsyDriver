#ifndef __EVGENY_UTILITIES_TIMERS__
#define __EVGENY_UTILITIES_TIMERS__

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time.hpp>

namespace evg {

// sleep for specified time and return user input key
//   works on boost::thread and cv::waitKey
class KeyTimer {
private:
    boost::thread th_thread;
    bool th_workedOk;
    int inputKey;
    unsigned int delay;
    void sleep_the_dream();
public:

    // start the timer on ms delay
    void start(const unsigned int ms);
    // wait for the timer and return the user input if any
    int wait();
};


// sleep for specified time
//   works on boost::thread and boost::date_time
class Timer {
private:
    boost::thread th_thread;
    bool th_running;
    unsigned int delay;
    void sleep_the_dream();
public:

    // start the timer on ms delay
    void start(const unsigned int ms);
    // wait for the timer and return
    void wait();
};

} // namespace evg

#endif
