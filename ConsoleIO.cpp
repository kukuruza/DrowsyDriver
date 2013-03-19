#include <ConsoleIO.h>
#include <string>
#include <iostream>
#include <exception>
#include <vector>

using namespace std;
using namespace TCLAP;


// alternatives for detector
ValuesConstraint<string> makeAllowedDetectorVals()
{
    // the set of values of detector
    vector<string> detectorVect;
    detectorVect.push_back("default");
    detectorVect.push_back("1");
    detectorVect.push_back("2");
    detectorVect.push_back("opencv");
    detectorVect.push_back("kyle");
    return ValuesConstraint<string>(detectorVect);
}


// alternatives for tracker
ValuesConstraint<string> makeAllowedTrackerVals()
{
    vector<string> trackerVect;
    trackerVect.push_back("default");
    trackerVect.push_back("1");
    trackerVect.push_back("2");
    trackerVect.push_back("3");
    trackerVect.push_back("4");
    trackerVect.push_back("optical_flow");
    trackerVect.push_back("camshift");
    trackerVect.push_back("compressive");
    trackerVect.push_back("kyle");
    return ValuesConstraint<string>(trackerVect);
}



bool consoleInput (const std::vector<std::string>& _args,
                   evg::SrcVideo& _inputVideo, std::string& _outputVideoPath,
                   DetectorType& _detectorType, const DetectorType defaultDetectorType,
                   TrackerType& _trackerType, const TrackerType defaultTrackerType,
                   double& _redetectionTime,
                   bool& _restart)
{
    try {
        ValuesConstraint<string> allowedDetectorVals( makeAllowedDetectorVals() );
        ValuesConstraint<string> allowedTrackerVals( makeAllowedTrackerVals() );

        // make parsing construction
        //   4th arg 'false' disables help. Help results in exit in TCLAP
        CmdLine cmd("Set/reset all parameters", ' ', "0", false);
        ValueArg<string> cmdDetector ("d", "detector", "number or name of detector.",
                                           false, "default", &allowedDetectorVals, cmd);
        ValueArg<string> cmdTracker  ("t", "tracker", "number or name of tracker",
                                           false, "default", &allowedTrackerVals, cmd);
        ValueArg<double> cmdPeriod   ("p", "period", "redetection period, in range [0; 4]",
                                           false, 2., "double", cmd);
        ValueArg<string> cmdInput    ("i", "input", "input video file path",
                                           false, "", "string", cmd);
        SwitchArg cmdCamera          ("c", "camera", "indicate when using camera", cmd, false);
        ValueArg<string> cmdOutput   ("o", "output", "output video path",
                                           false, "", "string", cmd);
        SwitchArg cmdRestart         ("r", "restart", "restart the video", cmd, false);

        // parse
        //   for some reason TCLAP does not have the const qualifier
        std::vector<std::string> args = _args;
        cmd.parse(args);


        // parse restart
        if (cmdRestart.isSet())
            _restart = true;
        else _restart = false;

        // parse inputVideo
        // if input method is set, we restart
        if (cmdCamera.isSet() || cmdInput.isSet())
            _restart = true;
        // set the new input methd
        if (cmdCamera.isSet())
            _inputVideo = evg::SrcVideo(evg::SrcVideo::CAMERA, "");
        else if (cmdInput.isSet())
            _inputVideo = evg::SrcVideo(evg::SrcVideo::FILE, cmdInput.getValue());
        // no way to set mutually exclusive but non-required arguments => check manually
        if (cmdCamera.isSet() && cmdInput.isSet())
        {
            cerr << "Input from both camera and file cannot be specified." << endl;
            return 0;
        }

        // parse outputVideoPath
        if (cmdOutput.isSet())
            _outputVideoPath = cmdOutput.getValue();

        // parse detectorType
        if (cmdDetector.isSet())
        {
            if (cmdDetector.getValue() == "default")
                _detectorType = defaultDetectorType;
            else if (cmdDetector.getValue() == "1" || cmdDetector.getValue() == "opencv")
                _detectorType = OPENCV_DETECTOR;
            else if (cmdDetector.getValue() == "2" || cmdDetector.getValue() == "kyle")
                _detectorType = KYLE_DETECTOR;
            else assert(0);
        }

        // parse trackerType
        if (cmdTracker.isSet())
        {
            if (cmdTracker.getValue() == "default")
                _trackerType = defaultTrackerType;
            else if (cmdTracker.getValue() == "1" || cmdTracker.getValue() == "optical_flow")
                _trackerType = OPTICAL_FLOW_TRACKER;
            else if (cmdTracker.getValue() == "2" || cmdTracker.getValue() == "camshift")
                _trackerType = CAMSHIFT_TRACKER;
            else if (cmdTracker.getValue() == "3" || cmdTracker.getValue() == "compressive")
                _trackerType = COMPRESSIVE_TRACKER;
            else if (cmdTracker.getValue() == "4" || cmdTracker.getValue() == "kyle")
                _trackerType = KYLE_TRACKER;
            else assert(0);
        }

        // parse redetection time
        if (cmdPeriod.isSet())
            _redetectionTime = cmdPeriod.getValue();

        return 1;
    } catch (...) {
        std::cerr << "consoleInput(): exception caught" << std::endl;
        return 0;
    }
}


bool consoleFirstInput (int argc, char *argv[],
                        evg::SrcVideo& _inputVideo, std::string& _outputVideoPath,
                        DetectorType& _detectorType, const DetectorType defaultDetectorType,
                        TrackerType& _trackerType, const TrackerType defaultTrackerType,
                        double& _redetectionTime)
{
    try {
        ValuesConstraint<string> allowedDetectorVals( makeAllowedDetectorVals() );
        ValuesConstraint<string> allowedTrackerVals( makeAllowedTrackerVals() );

        // make parsing construction
        CmdLine cmd("Set/reset all parameters", ' ', "0");
        ValueArg<string> cmdDetector ("d", "detector", "number or name of detector.",
                                           false, "default", &allowedDetectorVals, cmd);
        ValueArg<string> cmdTracker  ("t", "tracker", "number or name of tracker",
                                           false, "default", &allowedTrackerVals, cmd);
        ValueArg<double> cmdPeriod   ("p", "period", "redetection period, in range [0; 4]",
                                           false, 2., "double", cmd);
        ValueArg<string> cmdInput    ("i", "input", "input video file path",
                                           false, "", "string");
        SwitchArg cmdCamera ("c", "camera", "indicate when using camera", false);
        cmd.xorAdd( cmdInput, cmdCamera );
        ValueArg<string> cmdOutput  ("o", "output", "output video path",
                                           false, "", "string", cmd);

        // parse
        cmd.parse(argc, argv);


        // parse inputVideo
        if (cmdCamera.isSet())
            _inputVideo = evg::SrcVideo(evg::SrcVideo::CAMERA, "");
        else if (cmdInput.isSet())
            _inputVideo = evg::SrcVideo(evg::SrcVideo::FILE, cmdInput.getValue());
        else
            assert(0);

        // parse outputVideoPath
        _outputVideoPath = cmdOutput.getValue();

        // parse detectorType
        if (cmdDetector.isSet())
        {
            if (cmdDetector.getValue() == "default")
                _detectorType = defaultDetectorType;
            else if (cmdDetector.getValue() == "1" || cmdDetector.getValue() == "opencv")
                _detectorType = OPENCV_DETECTOR;
            else if (cmdDetector.getValue() == "2" || cmdDetector.getValue() == "kyle")
                _detectorType = KYLE_DETECTOR;
            else assert(0);
        }
        else
            _detectorType = defaultDetectorType;

        // parse trackerType
        if (cmdTracker.isSet())
        {
            if (cmdTracker.getValue() == "default")
                _trackerType = defaultTrackerType;
            else if (cmdTracker.getValue() == "1" || cmdTracker.getValue() == "optical_flow")
                _trackerType = OPTICAL_FLOW_TRACKER;
            else if (cmdTracker.getValue() == "2" || cmdTracker.getValue() == "camshift")
                _trackerType = CAMSHIFT_TRACKER;
            else if (cmdTracker.getValue() == "3" || cmdTracker.getValue() == "compressive")
                _trackerType = COMPRESSIVE_TRACKER;
            else if (cmdTracker.getValue() == "4" || cmdTracker.getValue() == "kyle")
                _trackerType = KYLE_TRACKER;
            else assert(0);
        }
        else
            _trackerType = defaultTrackerType;


        // parse redetection time
        if (cmdPeriod.isSet())
            _redetectionTime = cmdPeriod.getValue();
        else
            _redetectionTime = 2.;


        return 1;
    } catch (...) {
        std::cerr << "consoleFirstInput(): exception caught" << std::endl;
        return 0;
    }
}


bool checkFirstInput(int argc, char *argv[])
{
    try {
        ValuesConstraint<string> allowedDetectorVals( makeAllowedDetectorVals() );
        ValuesConstraint<string> allowedTrackerVals( makeAllowedTrackerVals() );

        // make parsing construction
        CmdLine cmd("Set/reset all parameters", ' ', "0");
        ValueArg<string> cmdDetector ("d", "detector", "number or name of detector.",
                                           false, "default", &allowedDetectorVals, cmd);
        ValueArg<string> cmdTracker  ("t", "tracker", "number or name of tracker",
                                           false, "default", &allowedTrackerVals, cmd);
        ValueArg<double> cmdPeriod   ("p", "period", "redetection period, in range [0; 4]",
                                           false, 2., "double", cmd);
        ValueArg<string> cmdInput  ("i", "input", "input video file path",
                                           false, "", "string");
        SwitchArg cmdCamera ("c", "camera", "indicate when using camera", false);
        cmd.xorAdd( cmdInput, cmdCamera );
        ValueArg<string> cmdOutput  ("o", "output", "output video path",
                                           false, "", "string", cmd);

        // parse
        cmd.parse(argc, argv);

        // that's all. If we get here, the input is ok.

        return 1;
    } catch (...) {
        return 0;
    }
}


bool printParameters (const evg::SrcVideo& _inputVideo, const std::string& _outputVideoPath,
                      const DetectorType _detectorType, const TrackerType _trackerType,
                      const double _redetectionTime)
{
    try {
        cout << "Enter 'q' to quit, '-r' to restart" << endl;
        cout << "Active parameters:" << endl
             << "    input        [-c / -i] "
             << (_inputVideo.getType() == evg::SrcVideo::CAMERA ? "camera" : _inputVideo.getPath()) << endl
             << "    output            [-o] "
             << (_outputVideoPath == "" ? "N/A" : _outputVideoPath) << endl
             << "    detector type     [-d] " << writeDetectorType(_detectorType) << endl
             << "    tracker type      [-t] " << writeTrackerType(_trackerType) << endl
             << "    redetection time  [-p] " << _redetectionTime << endl;
        return 1;
    } catch (...) {
        std::cerr << "consolePrintout(): exception caught" << std::endl;
        return 0;
    }
}




