#ifndef EVGENY_UTILITIES_MEDIA_LOAD_SAVE
#define EVGENY_UTILITIES_MEDIA_LOAD_SAVE

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace evg {
    


// functions with return may throw exceptions,
// boolean versions are no-except. They return 1 on success, 0 on failure
//

cv::Mat loadImage (const std::string& imagePath);
bool    loadImage (const std::string& imagePath, cv::Mat& image);

cv::Mat undistortImage     (const std::string& calibrationPath, const cv::Mat& image);
bool    undistortImageBool (const std::string& calibrationPath, cv::Mat& image);

cv::VideoCapture openVideo (const std::string& videoPath);
bool    openVideo (const std::string& videoPath, cv::VideoCapture& video);

// open a video for write based on input video parameters
cv::VideoWriter newVideo (const std::string& videoOutPath, const cv::VideoCapture& videoIn);
bool    newVideo (const std::string& _videoOutPath, const cv::VideoCapture& _videoIn,
                  cv::VideoWriter& videoOut);
    
void    debugImage     (const cv::Mat& image);
bool    debugImageBool (const cv::Mat& image);

// delimiter is always space now
// matrix will be always CV_64F on output, it will put zeros for missing values
cv::Mat dlmread (const std::string& dlmfilePath);
bool    dlmread (const std::string& dlmfilePath, cv::Mat& matrix);

bool    loadCPs (const std::string& CPsFilePath,
                 std::vector<cv::Point2f>& refPoints,
                 std::vector<cv::Point2f>& newPoints);


    
// to use when video data can be from either camera or file
class SrcVideo {
public:
    enum Type { NOT_SET = -1, CAMERA = 0, FILE = 1 };
private:
    Type type;
    std::string videoPath;
    // your favorite camera resoluton, const
    unsigned int CameraWidth, CameraHeight;
public:
    friend bool operator== (const SrcVideo&, const SrcVideo&);
    friend bool operator!= (const SrcVideo&, const SrcVideo&);
    SrcVideo(const Type _type = NOT_SET, const std::string _videoPath = "");
    bool openResource (cv::VideoCapture& video);
    // goal of closeResource() is being exception-free and managing video within class
    bool closeResource (cv::VideoCapture& video);
    inline const Type getType() const { return type; }
    inline const std::string& getPath() const { return videoPath; }
};
inline bool operator== (const SrcVideo& a, const SrcVideo& b)
{ return (a.type == b.type && a.videoPath == b.videoPath &&
          a.CameraWidth == b.CameraWidth && a.CameraHeight == b.CameraHeight); }
inline bool operator!= (const SrcVideo& a, const SrcVideo& b)
{ return !(a == b); }
    
    
} // namespace evg

#endif
