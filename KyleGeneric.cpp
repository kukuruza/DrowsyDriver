#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/filesystem.hpp>
#include "evgUtilities/rectOperations.h"
//
// essential that KyleGeneric is included after opencv2
#include "KyleGeneric.h"


using namespace cv;
using namespace std;



KyleGeneric::KyleGeneric() :

    nIter  (5),
    clamp  (3),
    fTol   (0.01),
    fcheck (false),
    fpd    (-1)
{
    expandFacePerc = vector<double>(4);
    expandFacePerc[0] = 0.;
    expandFacePerc[1] = 0.3;
    expandFacePerc[2] = 0.;
    expandFacePerc[3] = 0.;
    expandEyesPerc = vector<double>(4);
    expandEyesPerc[0] = 0.2;
    expandEyesPerc[1] = 0.7;
    expandEyesPerc[2] = 0.2;
    expandEyesPerc[3] = 0.7;

    //Kyle face detector//
    wSize1.push_back(7);
    //
    wSize.push_back(11);
    wSize.push_back(9);
    wSize.push_back(7);

    string ftFile = string(KYLE_MODEL_PATH) + "/model/face2.tracker";
    string triFile = string(KYLE_MODEL_PATH) + "/model/face.tri";
    string conFile = string(KYLE_MODEL_PATH) + "/model/face.con";

    if (! boost::filesystem::exists(ftFile) ||
        ! boost::filesystem::exists(triFile) ||
        ! boost::filesystem::exists(conFile) )
    {
        cerr << "Paths ftFile, triFile, or conFile does not exist:" << endl
             << ftFile.c_str() << endl
             << triFile.c_str() << endl
             << conFile.c_str() << endl;
        throw exception();
    }

    model = new FACETRACKER::Tracker(ftFile.c_str());
    tri=FACETRACKER::IO::LoadTri(triFile.c_str());
    con=FACETRACKER::IO::LoadCon(conFile.c_str());

    failed_kyle=true;

}


KyleGeneric::~KyleGeneric()
{
    delete model;
}


bool KyleGeneric::processFace (const Size& _imageSize, Face& face)
{
    try {


        //Setting the face parameters
        int idx = (model->_clm).GetViewIdx();
        //
        face.face_rectangle = model->_rect;
        face.shape=model->_shape;
        face.con=con;
        face.tri=tri;
        face.visi = (model->_clm)._visi[idx];

        // extract eye rectangles from points
        //
        // get 6 and 6 points from face.shape, rows = 36-41 and rows = 42-47
        const int eye1start = 36, eye2start = 42, eyelength = 6;
        //
        // x and y are stored in the same double size vector
        int numPoints = face.shape.rows/2;
        //
        // collect eye points
        vector<Point> eye1set, eye2set, faceSet;
        for (int i = eye1start; i < eye1start + eyelength; i++)
        {
            // skip the invisible points (turned head)
            if (face.visi.at<int>(i,0) == 0) continue;
            // collect a point from data
            Point p = Point( face.shape.at<double>(i,0),
                             face.shape.at<double>(i+numPoints,0) );
            eye1set.push_back(p);
        }
        for (int i = eye2start; i < eye2start + eyelength; i++)
        {
            // skip the invisible points (turned head)
            if (face.visi.at<int>(i,0) == 0) continue;
            // collect a point from data
            Point p = Point( face.shape.at<double>(i,0),
                             face.shape.at<double>(i+numPoints,0) );
            eye2set.push_back(p);
        }
        for (int i = 0; i < numPoints; i++)
        {
            // skip the invisible points (turned head)
            if (face.visi.at<int>(i,0) == 0) continue;
            // collect a point from data
            Point p = Point( face.shape.at<double>(i,0),
                             face.shape.at<double>(i+numPoints,0) );
            faceSet.push_back(p);
        }
        //
        face.eye_rectangles.clear();
        //
        // only take eyes with existing points
        if (eye1set.size() > 0)
        {
            Rect rect1 = boundingRect (eye1set);
            // write rectangles to the Face object
            face.eye_rectangles.push_back( rect1 );
        }
        if (eye2set.size() > 0)
        {
            Rect rect2 = boundingRect (eye2set);
            // write rectangles to the Face object
            face.eye_rectangles.push_back( rect2 );
        }
        if (faceSet.size() > 0)
        {
            Rect rect3 = boundingRect (faceSet);
            // write rectangles to the Face object
            face.face_rectangle = rect3;
        }
        //
        // expand every rectangle for better stability
        const int FaceMinSize = 10;
        face.face_rectangle = evg::expandRectPercRect (face.face_rectangle, expandFacePerc,
                                                       _imageSize, FaceMinSize);

        const int EyeMinSize = 10;
        for (unsigned int i = 0; i != face.eye_rectangles.size(); ++i)
            face.eye_rectangles[i] = evg::expandRectPercRect (face.eye_rectangles[i], expandEyesPerc,
                                                              _imageSize, EyeMinSize);

        return 1;
    } catch(...) {
        cerr << "Error in KyleGeneric::processFace()." << endl;
        return 0;
    }
}
