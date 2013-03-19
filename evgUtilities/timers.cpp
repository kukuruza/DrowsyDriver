#include "timers.h"

using namespace std;


/////// ---- evgKeyTimer ---- ///////


void evg::KeyTimer::sleep_the_dream()
{
    try {
        inputKey = cv::waitKey(delay);
        th_workedOk = true;
    } catch(...) {
        cerr << "evgKeyTimer::sleep_the_dream(): exception is caught." << endl;
        th_workedOk = false;
    }
}

void evg::KeyTimer::start(const unsigned int _ms)
{
    try {
        delay = _ms;
        th_thread = boost::thread (&evg::KeyTimer::sleep_the_dream, this);
    } catch (boost::thread_interrupted&) {
        cout << "evg::KeyTimer::sleep_the_dream() was interrupted." << endl;
    } catch(...) {
        cerr << "evg::KeyTimer::start(): cannot start the thread." << endl;
    }
}

int evg::KeyTimer::wait()
{
    try {
        if (th_thread.joinable()) th_thread.join();
        if (! th_workedOk) return 0;
        return inputKey;
    } catch (boost::thread_interrupted&) {
        // if we caught this, we need to finish quick
        th_thread.interrupt();
        if (th_thread.joinable()) th_thread.join();
        // give interruption back to the function that called evgTimer
        throw boost::thread_interrupted();
    } catch(...) {
        cerr << "evg::KeyTimer::wait() cannot join the thread." << endl;
        return -1;
    }
}



/////// ---- evgTimer ---- ///////


void evg::Timer::sleep_the_dream()
{
    try {
        boost::posix_time::milliseconds workTime(delay);
        boost::this_thread::sleep(workTime);
    } catch (boost::thread_interrupted&) {
        cout << "evg::Timer::sleep_the_dream() was interrupted." << endl;
    } catch(...) {
        cerr << "evg::Timer::sleep_the_dream(): exception is caught." << endl;
    }
}

void evg::Timer::start(const unsigned int _ms)
{
    try {
        delay = _ms;
        th_thread = boost::thread (&evg::Timer::sleep_the_dream, this);
    } catch(...) {
        cerr << "evg::Timer::start(): cannot start the thread." << endl;
    }
}

void evg::Timer::wait()
{
    try {
        if (th_thread.joinable()) th_thread.join();
    } catch (boost::thread_interrupted&) {
        // if we caught this, we need to finish quick
        th_thread.interrupt();
        if (th_thread.joinable()) th_thread.join();
        // give interruption back to the function that called evgTimer
        throw boost::thread_interrupted();
    } catch(...) {
        cerr << "evg::Timer::wait() cannot join the thread." << endl;
    }
}
