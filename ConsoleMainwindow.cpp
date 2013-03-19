#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ConsoleMainwindow.h"

using namespace std;
using namespace cv;


bool ConsoleMainwindow::getInput()
{
    try {
        // get input from std::cin
        string line;
        if (!getline(cin, line)) return 0;

        // check if user wants to finish
        if (line == "q" || line == "-q" || line == "Q")
        {
            stopRequest = true;
            return 1;
        }
        else
            stopRequest = false;

        // split input into words
        istringstream osline(line);
        string word;
        vector<string> args;
        // first argument for tclap parser is the program name
        args.push_back("DrowsyDriver");
        while (osline >> word)
            args.push_back(word);

        // parse the input (give some current values as default).
        //   bad input is ok
        consoleInput (args,
                      newInput, newOutputVideoPath,
                      newDetectorType, cvmanager.detectorType,
                      newTrackerType, cvmanager.trackerType,
                      newRedetectionTime,
                      restartRequest);

        return 1;
    } catch(...) {
        cerr << "ConsoleMainwindow::getInput(): exception caught." << endl;
        return 0;
    }
}


void ConsoleMainwindow::initCVmanager()
{
    cvmanager.start();
}

void ConsoleMainwindow::resumeCVmanager()
{
    cvmanager.prepareLoop();
    cvmanager.runLoop();
}

void ConsoleMainwindow::pauseCVmanager()
{
    cvmanager.stopLoop();
}

void ConsoleMainwindow::stopCVmanager()
{
    cvmanager.stopLoop();
    cvmanager.finish();
}


void ConsoleMainwindow::initInterface(int argc, char *argv[])
{
    try {
        if (! consoleFirstInput (argc, argv,
                                 newInput, newOutputVideoPath,
                                 newDetectorType, cvmanager.defaultDetectorType,
                                 newTrackerType, cvmanager.defaultTrackerType,
                                 newRedetectionTime)
           ) throw exception();

        cvmanager.input = newInput;
        cvmanager.outputPath = newOutputVideoPath;
        cvmanager.detectorType = newDetectorType;
        cvmanager.trackerType = newTrackerType;
        cvmanager.setRedetectionPeriod(newRedetectionTime);

        initCVmanager();
        resumeCVmanager();

    } catch(...) {
        cerr << "ConsoleMainwindow::initInterface(): exception caught." << endl;
        throw std::exception();
    }
}


void ConsoleMainwindow::runInterface()
{
    try {
        syncOk = false;
        while(1)
        {
            // show the current settings
            printParameters( cvmanager.input, cvmanager.outputPath,
                             cvmanager.detectorType, cvmanager.trackerType,
                             cvmanager.getRedetectionPeriodExp() );

            // set to current input
            newInput = cvmanager.input;
            newOutputVideoPath = cvmanager.outputPath;
            newDetectorType = cvmanager.detectorType;
            newTrackerType = cvmanager.trackerType;
            newRedetectionTime = cvmanager.getRedetectionPeriodExp();

            // wait for input from std::cin
            getInput();

            // exit condition
            if (stopRequest)
            {
                cout << "ConsoleMainwindow::runInterface(): exiting" << endl;
                syncIsOn = false;
                exitRequest = true;
                stopCVmanager();
                return;
            }

            // if full stop is required
            if (newInput != cvmanager.input ||
                newOutputVideoPath != cvmanager.outputPath ||
                restartRequest)
            {
                cout << "ConsoleMainwindow::runInterface(): re-starting" << endl;
                syncIsOn = false;
                restartRequest = false;
                pauseCVmanager();
                stopCVmanager();
                cout << "here" << endl;
                if (cvmanager.input != newInput)
                    cvmanager.input = newInput;
                if (cvmanager.outputPath != newOutputVideoPath)
                    cvmanager.outputPath = newOutputVideoPath;
                if (cvmanager.detectorType != newDetectorType)
                    cvmanager.detectorType = newDetectorType;
                if (cvmanager.trackerType != newTrackerType)
                    cvmanager.trackerType = newTrackerType;
                if (cvmanager.getRedetectionPeriodExp() != newRedetectionTime)
                    cvmanager.setRedetectionPeriod(newRedetectionTime);
                initCVmanager();
                resumeCVmanager();
                syncIsOn = true;
                cout << "ConsoleMainwindow::runInterface(): resumed" << endl;
            }
            // if pause is required
            else if ( cvmanager.detectorType != newDetectorType ||
                      cvmanager.trackerType != newTrackerType )
            {
                cout << "ConsoleMainwindow::runInterface(): pausing" << endl;
                pauseCVmanager();
                if (cvmanager.detectorType != newDetectorType)
                    cvmanager.detectorType = newDetectorType;
                if (cvmanager.trackerType != newTrackerType)
                    cvmanager.trackerType = newTrackerType;
                if (cvmanager.getRedetectionPeriodExp() != newRedetectionTime)
                    cvmanager.setRedetectionPeriod(newRedetectionTime);
                resumeCVmanager();
            }
            // if can go on the fly
            else if (cvmanager.getRedetectionPeriodExp() != newRedetectionTime)
                cvmanager.setRedetectionPeriod(newRedetectionTime);
        }
        syncOk = true;
    } catch(...) {
        cerr << "ConsoleMainwindow::runInterface(): exception caught." << endl;
    }
}


bool ConsoleMainwindow::run()
{
    try {
        syncIsOn = true;
        syncThread = boost::thread(&ConsoleMainwindow::runInterface, this);

        // displaying is in the main thread because of the way cv::imshow() works
        namedWindow("Drowsy Driver");
        exitRequest = false;
        while(exitRequest == false)
            if (syncIsOn == true)
            {
                // braces are mandatory for 'lock'
                {
                    // Acquire lock on the queue
                    boost::unique_lock<boost::mutex> lock(cvmanager.frame_mutex);
                    cvmanager.frame_notifier.wait(lock);
                    image_aux = cvmanager.showFrame.clone();
                }
                //
                imshow("Drowsy Driver", image_aux);
                // without waitKey() displayed image won't update
                waitKey(10);
            }

        syncThread.join();

        return 1;
    } catch(...) {
        cerr << "ConsoleMainwindow::run(): exception caught." << endl;
        return 0;
    }
}


void ConsoleMainwindow::stopInterface()
{
    try {
        stopCVmanager();
    } catch(...) {
        cerr << "ConsoleMainwindow::stopInterface(): exception caught." << endl;
    }
}



