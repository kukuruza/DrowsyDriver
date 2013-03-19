#ifndef CONSOLEMAINWINDOW_H
#define CONSOLEMAINWINDOW_H

#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include "ConsoleIO.h"
#include "boost/thread.hpp"
#include "opencv2/opencv.hpp"
#include <CVmanager.h>


class ConsoleMainwindow {
private:
    CVmanager cvmanager;

    // framework for updating screen
    bool syncIsOn, syncOk;
    boost::thread syncThread;
    cv::Mat image_aux;

    bool getInput();
    bool restartRequest;
    bool exitRequest;

    // operations on CVmanager (used with setting/resetting stuff)
    void initCVmanager();
    void pauseCVmanager();
    void resumeCVmanager();
    void stopCVmanager();

    // input from console goes here. It is set to current settings before re-input
    evg::SrcVideo  newInput;
    std::string    newOutputVideoPath;
    DetectorType   newDetectorType;
    TrackerType    newTrackerType;
    double         newRedetectionTime;

    bool stopRequest;

    void initInterface(int argc, char *argv[]);
    void stopInterface();
    void runInterface();

public:
    ConsoleMainwindow(int argc, char *argv[]) { initInterface(argc, argv); }
    ~ConsoleMainwindow()                      { stopInterface(); }
    bool run();
};


#endif // CONSOLEMAINWINDOW_H
