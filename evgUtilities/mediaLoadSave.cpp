#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "mediaLoadSave.h"

using namespace std;
using namespace cv;
using namespace boost::filesystem;


Mat evg::loadImage (const std::string& imagePath)
{
    path p(imagePath);
    if (! exists(p) )
    {
        std::cerr << "evg::loadImage(): Path '" << absolute(imagePath)
                  << "' does not exist." << std::endl;
        throw std::exception();
    }
    cv::Mat image = cv::imread(imagePath, CV_LOAD_IMAGE_COLOR);
    if (! image.data )
    {
        std::cerr << "Image '" << absolute(imagePath) << "' failed to open." << std::endl;
        throw std::exception();
    }
    return image;
}


bool evg::loadImage (const std::string& imagePath, cv::Mat& image)
{
    try {
        image = loadImage(imagePath);
        return 1;
    } catch(...) { return 0; }
}


Mat evg::undistortImage(const std::string& calibrationPath, const Mat& _image)
{
    // open calibration file
    path p(calibrationPath);
    if (! exists(p) )
    {
        cerr << "evg::calibImage(): Path '" << absolute(calibrationPath)
             << "' does not exist." << endl;
        throw exception();
    }
    FileStorage fs(calibrationPath.c_str(), FileStorage::READ);
    if (! fs.isOpened() )
    {
        cerr << "evg::calibImage(): Calibration file '" << absolute(calibrationPath)
             << "' failed to open." << endl;
        throw exception();
    }
    
    // read calibration file
    Mat cameraMatrix, distCoeffs;
    fs["Camera_Matrix"] >> cameraMatrix;
    fs["Distortion_Coefficients"] >> distCoeffs;
    fs.release();
    
    // undistort image
    Mat resultImage;
    undistort (_image, resultImage, cameraMatrix, distCoeffs);
    
    return resultImage;
}


bool evg::undistortImageBool (const string& calibrationPath, Mat& image)
{
    try {
        image = undistortImage(calibrationPath, image);
        return 1;
    } catch (...) { return 0; }
}


VideoCapture evg::openVideo(const string& videoPath)
{
    path p(videoPath);
    if (! exists(p) )
    {
        cerr << "evg::openVideo(): Video path '" << absolute(videoPath)
             << "' does not exist." << endl;
        throw exception();
    }
    
    VideoCapture video;
    if (! video.open(videoPath) )
    {
        cerr << "evg::openVideo(): Video '" << absolute(videoPath)
             << "' failed to open." << endl;
        throw exception();
    }
    
    return video;
}


bool evg::openVideo(const string& videoPath, VideoCapture& _video)
{
    try {
        _video = openVideo(videoPath);
        return 1;
    } catch (...) { return 0; }
}
    
    
cv::VideoWriter evg::newVideo (const std::string& _videoOutPath, const cv::VideoCapture& _videoIn)
{
    // get parameters from the input video
    cv::VideoCapture videoIn = _videoIn;
    int codec = (int)(videoIn.get(CV_CAP_PROP_FOURCC));
    double fps = videoIn.get(CV_CAP_PROP_FPS);
    const Size frameSize ( (int)(videoIn.get(CV_CAP_PROP_FRAME_WIDTH)),
                           (int)(videoIn.get(CV_CAP_PROP_FRAME_HEIGHT)) );
    
    // in case of camera input
    // set default frame rate
    const int DefaultFps = 30;
    if (fps == 0) fps = DefaultFps;
    // set default code (combine ascii code of each of four letter into an int)
    const char* DefaultCodec = "XVID";
    if (codec == 0) codec = (DefaultCodec[0] << 24) + (DefaultCodec[1] << 16)
                          + (DefaultCodec[2] << 8) + DefaultCodec[3];
    
    cout << "evg::newVideo(): codec code = " << codec << endl;
    cout << "                 frame rate = " << fps << endl;
    cout << "                 frame size = [" << frameSize.width << " x "
         << frameSize.height << "]" << endl;

    // check the parent path for output video
    path p(_videoOutPath);
    if (! p.has_parent_path())
    {
        cerr << "evg::newVideo(): Video path '" << absolute(_videoOutPath)
             << "has no parent directory" << endl;
        throw exception();
    }
    if (! exists(p.parent_path()) )
    {
        cerr << "evg::newVideo(): Video directory path '" << p.parent_path()
             << "' does not exist." << endl;
        throw exception();
    }

    // open video for output
    VideoWriter videoOut;
    if (! videoOut.open(_videoOutPath, codec, fps, frameSize) )
    {
        cerr << "evg::newVideo(): Video '" << absolute(_videoOutPath)
             << "' failed to open." << endl;
        throw exception();
    }
    
    return videoOut;
}


bool evg::newVideo (const string& _videoOutPath, const VideoCapture& _videoIn, VideoWriter& videoOut)
{
    try {
        videoOut = evg::newVideo(_videoOutPath, _videoIn);
        return 1;
    } catch (...) { return 0; }
}


void evg::debugImage (const cv::Mat& image)
{
    cout << "test: " << image << endl;
    namedWindow("test", CV_WINDOW_AUTOSIZE);
    imshow("test", image);
    while (1)
        if ( cv::waitKey(20) == 27 )
            throw std::exception();
}


bool evg::debugImageBool (const cv::Mat& image)
{
    try {
        debugImage(image);
        return 1;
    } catch(...) { return 0; }
}



// delimiter is always space now
// matrix will be always CV_64F on output, it will put zeros for missing values
Mat evg::dlmread (const std::string& dlmfilePath)
{
    // open file
    path p(dlmfilePath);
    if (! exists(p) )
    {
        cerr << "evg::dlmread(): Path '" << absolute(dlmfilePath)
             << "' does not exist." << endl;
        throw exception();
    }
    ifstream fileStream (dlmfilePath.c_str());
    if (!fileStream.good())
    {
        cerr << "evg::dlmread(): File '" << absolute(dlmfilePath)
             << "' failed to open." << endl;
        throw exception();
    }
    
    // read file to compute the matrix size [numRows, numCols]
    string line;
    istringstream iss;
    unsigned int row = 0, col = 0, numRows = 0, numCols = 0;
    for (row = 0; getline(fileStream, line); ++row)
    {
        // process the empty line in the end of file
        if (line == "") { --row; break; }
        iss.clear();
        iss.str(line);
        double dummy;
        for (col = 0; (iss >> dummy); ++col)
            ;
        if (col > numCols) numCols = col;
    }
    numRows = row;
    if(fileStream.bad() || iss.bad())
    {
        std::cerr << "evg::dlmread(): error reading the file." << std::endl;
        throw exception();
    }
    
    
    // rewind the file
    fileStream.clear();
    fileStream.seekg(ios_base::beg);
    
    // create the matrix of result size
    Mat matrix = Mat::zeros(numRows, numCols, CV_64F);
    
    // read file and put values into Mat
    double num;
    for (int row = 0; getline(fileStream, line) && row != matrix.rows; ++row)
    {
        iss.clear();
        iss.str(line);
        for (int col = 0; col != matrix.cols && (iss >> num); ++col)
            matrix.at<double>(row, col) = num;
    }
    if(fileStream.bad() || iss.bad())
    {
        std::cerr << "evg::dlmread(): error reading the file." << std::endl;
        throw exception();
    }
    
    return matrix;
}


bool evg::dlmread (const std::string& dlmfilePath, cv::Mat& _matrix)
{
    try {
        _matrix = dlmread (dlmfilePath);
        return 1;
    } catch(...) { return 0; }
}


bool evg::loadCPs (const string& CPsFilePath,
              vector<Point2f>& refPoints, vector<Point2f>& newPoints)
{
    try {
        refPoints.clear();
        newPoints.clear();
        
        Mat allMat = dlmread(CPsFilePath);
        
        if (allMat.cols != 4)
        {
            cerr << "evg::loadCPs(): number of columns != 4 in the file." << endl;
            return 0;
        }
        
        for (int row = 0; row != allMat.rows; ++row)
        {
            refPoints.push_back( Point2f((float)allMat.at<double>(row, 0),
                                         (float)allMat.at<double>(row, 1)) );
            newPoints.push_back( Point2f((float)allMat.at<double>(row, 2),
                                         (float)allMat.at<double>(row, 3)) );
        }
        
        return 1;
    } catch(...) { return 0; }
}


evg::SrcVideo::SrcVideo(const Type _type, const std::string _videoPath)
  : type (_type), videoPath(_videoPath),
    CameraWidth(640), CameraHeight(480)
{
    if (type == FILE && videoPath == "")
        std::cerr << "warning: evg::SrcVideo::SrcVideo(): input is set as file, "
                  << "but file path is not specified." << std::endl;
}


bool evg::SrcVideo::openResource(VideoCapture& video)
{
    try {
        if (video.isOpened()) return 1;
        
        if (type == evg::SrcVideo::CAMERA)
        {
            if (! video.open(0))
            {
                cerr << "evg::SrcVideo::openResource(): Camera failed to open." << endl;
                return 0;
            }
            video.set(CV_CAP_PROP_FRAME_WIDTH, CameraWidth);
            video.set(CV_CAP_PROP_FRAME_HEIGHT, CameraHeight);
            cout << "note: evg::SrcVideo::openResource(): width x height = "
                 << video.get(CV_CAP_PROP_FRAME_WIDTH) << " x "
                 << video.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
            cout << "note: evg::SrcVideo::openResource(): Camera successfully opened."
                 << endl;
        }
        else if (type == evg::SrcVideo::FILE)
        {
            if (! evg::openVideo(videoPath, video)) return 0;
        }
        else
        {
            cout << "evg::SrcVideo::openResource(): bad input type." << endl;
            return 0;
        }
        return 1;
    } catch (...) {
        cerr << "evg::SrcVideo::openResource(): excepton caught." << endl;
        return 0;
    }
}


bool evg::SrcVideo::closeResource(VideoCapture& video)
{
    try {
        if (! video.isOpened()) return 1;
        video.release();
        return 1;
    } catch(...) {
        cerr << "evg::SrcVideo::closeResource(): exception caught." << endl;
        return 0;
    }
}

