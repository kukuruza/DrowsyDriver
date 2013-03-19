#ifndef DrowsyDriver_ConsoleInput_h
#define DrowsyDriver_ConsoleInput_h

#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include "evgUtilities/mediaLoadSave.h"
#include "DetectingManager.h"
#include "TrackingManager.h"
#include <tclap/CmdLine.h>


// Read parameters from terminal. Not all the parameters are necessary
//   This function will be called on-the-fly every time a user enters something
bool consoleInput (const std::vector<std::string>& args,
                   evg::SrcVideo& _inputVideo, std::string& _outputVideoPath,
                   DetectorType& _detectorType, const DetectorType defaultDetectorType,
                   TrackerType& _trackerType, const TrackerType defaultTrackerType,
                   double& _redetectionTime,
                   bool& _restart);


// Read all parameters from terminal only in the beginning.
bool consoleFirstInput (int argc, char *argv[],
                        evg::SrcVideo& _inputVideo, std::string& _outputVideoPath,
                        DetectorType& _detectorType, const DetectorType defaultDetectorType,
                        TrackerType& _trackerType, const TrackerType defaultTrackerType,
                        double& _redetectionTime);


// Check the first input as the program starts. Returns 1 if input is ok.
bool checkFirstInput (int argc, char *argv[]);


// Printout active parameters to cout
bool printParameters (const evg::SrcVideo& _inputVideo, const std::string& _outputVideoPath,
                      const DetectorType _detectorType, const TrackerType _trackerType,
                      const double _redetectionTime);


#endif
