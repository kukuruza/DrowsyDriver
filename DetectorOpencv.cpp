#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "Common.h"

#include "DetectorOpencv.h"


OpencvDetector::OpencvDetector(){
    //Initializating the Opencv face detector
    face_frontal_cascade_name = std::string(OPENCV_DATA_PATH)
    + "/haarcascades/haarcascade_frontalface_alt.xml";

    face_profile_cascade_name = std::string(OPENCV_DATA_PATH)
    + "/haarcascades/haarcascade_profileface.xml";

    eyes_cascade_name = std::string(OPENCV_DATA_PATH)
    + "/haarcascades/haarcascade_eye_tree_eyeglasses.xml";


    //Loading the models
    if( !face_frontal_cascade.load(face_frontal_cascade_name))
    std::cerr<<"--(!)Error loading\n"<<std::endl;

    if( !face_profile_cascade.load(face_profile_cascade_name))
    std::cerr<<"--(!)Error loading\n"<<std::endl;

    if( !eyes_cascade.load( eyes_cascade_name ))
    std::cerr<<"--(!)Error loading\n"<<std::endl;



    face_winSize=200;
    eye_winSize=30;

}


bool OpencvDetector::detect(const cv::Mat& _image, Face& face)
{
    try {

        int number_profile_faces=0;

            // preprocess the image
        cv::Mat image;
        cvtColor( _image, image, CV_BGR2GRAY );
        equalizeHist( image, image );
        
            // detect faces
        std::vector<cv::Rect> faceRectVect;
        face_frontal_cascade.detectMultiScale(image, faceRectVect,
                                      1.1, 2, 0|CV_HAAR_SCALE_IMAGE,
                                      cv::Size(face_winSize, face_winSize));
        
        std::vector<cv::Rect> faceRectVect_profile;
        face_profile_cascade.detectMultiScale(image, faceRectVect_profile,
                                      1.1, 2, 0|CV_HAAR_SCALE_IMAGE,
                                      cv::Size(face_winSize, face_winSize));


        //Number of profiles faces
        number_profile_faces=faceRectVect_profile.size();
        //std::cout<<"faces frontal  "<<faceRectVect.size()
        //           <<"   faces profile "<<faceRectVect_profile.size() <<std::endl;


        // check if we actually detected anything
        if ((faceRectVect.size() + faceRectVect_profile.size())== 0 )
        {
            //std::cout << "OpencvDetector found no faces." << std::endl;
            return 0;
        }
        
        // take only the first face
        cv::Rect faceRect;

        if(faceRectVect_profile.size()>0)
            faceRect=faceRectVect_profile[0];
        else
            faceRect = faceRectVect[0];


        // get the face image
        cv::Mat faceROI = image(faceRect);
        
            // detect many eyes
        std::vector<cv::Rect> eyeRectVect;
        eyes_cascade.detectMultiScale(faceROI, eyeRectVect,
                                      1.1, 2, 0 | CV_HAAR_SCALE_IMAGE,
                                      cv::Size(eye_winSize, eye_winSize) );
        

        // correct for coordinates in ROI
        for (unsigned int i_eye = 0; i_eye != eyeRectVect.size(); ++i_eye)
        {
            eyeRectVect[i_eye].x = eyeRectVect[i_eye].x + faceRect.x;
            eyeRectVect[i_eye].y = eyeRectVect[i_eye].y + faceRect.y;
        }


        
        // we do not se any mechanism to verify the eyes.
        // therefore, we crop anything but the first 2 eyes
        if (eyeRectVect.size() > 2)
            eyeRectVect.erase(eyeRectVect.begin()+2, eyeRectVect.end());
        
            // write the result
        face.face_rectangle = faceRect;
        face.eye_rectangles = eyeRectVect;
        
        return 1;
    } catch(...) {
        std::cerr << "Error somewhere in OpencvDetector::detect()." << std::endl;
        return 0;
    }
}

