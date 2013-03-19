#include "Recognizer.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;




bool Recognizer::setFrame(const Mat& image)
{
    try {
        if (! image.data)
        {
            std::cerr << "Recognizer::setFrame(): Bad image." << std::endl;
            return 0;
        }
        if (image.type() != CV_8UC3)
        {
            std::cerr << "Recognizer::setFrame(): Bad image type." << std::endl;
            return 0;
        }
        cvtColor(image, grayImage, CV_RGB2GRAY);

        return 1;
    } catch(...) {
        cerr << "Recognizer::setFrame(): exception somewhere." << endl;
        return 0;
    }
}


void Recognizer::workFace()
{
    try {

        // recognizing face
        if (face.face_rectangle.size().area() == 0) return;
        currentFaceState = recogFace.classify(grayImage, face.face_rectangle);

        double factor = (currentFaceState == FACE_ON_THE_ROAD ? 0 : 1);
        integralFaceState = integralFaceState * (1 - faceInertia) +  factor* faceInertia;

    } catch(...) {
        std::cerr << "Error in Recognizer::workFace()." << std::endl;
    }
}


void Recognizer::workEyes()
{
    try {

        // recognizing eyes
        std::vector< cv::Rect* > eye_rects = face.asPtrVector();
        if ( eye_rects.size() > 3 )
        {
            std::cerr << "Error in Recognizer - more than 3 face rectangles." << std::endl;
            throw std::exception();
        }
        eye_rects.erase(eye_rects.begin());

        if (eye_rects.size() == 0) return;
        else if (eye_rects.size() == 1)
            currentEyeState = recogEyes.classify(grayImage, *eye_rects[0]);
        else if (eye_rects.size() == 2)
        {
            EyeClosedType eye1 = recogEyes.classify(grayImage, *eye_rects[0]);
            EyeClosedType eye2 = recogEyes.classify(grayImage, *eye_rects[1]);
            currentEyeState = (eye1 == EYE_CLOSED || eye2 == EYE_CLOSED ? EYE_CLOSED : EYE_OPEN);
        }
        else
            assert(false);

        double factor = (currentEyeState == EYE_OPEN ? 0 : 1);
        integralEyesState = integralEyesState * (1 - eyeInertia) + (factor) * eyeInertia;

    } catch(...) {
        std::cerr << "Error in Recognizer::workEyes()." << std::endl;
    }
}
