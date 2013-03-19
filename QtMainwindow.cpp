#include <iostream>
#include <boost/thread/thread.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "QtMainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace cv;


QtMainwindow::QtMainwindow(QWidget *parent) :

    QMainWindow(parent),
    ui(new Ui::MainWindow),

    DefaultOutVideoText ("video output path, if any")
{
    qRegisterMetaType< cv::Mat >("cv::Mat");
    qRegisterMetaType< FaceOnOffRoadType >("FaceOnOffRoadType");
    qRegisterMetaType< EyeClosedType >("EyeClosedType");
    qRegisterMetaType< float> ("float");
    qRegisterMetaType< QTextCursor > ("QTextCursor");
    qRegisterMetaType< QTextCharFormat > ("QTextCharFormat");

    ui->setupUi(this);

    // redirect cerr
    ui->coutBox->setReadOnly(true);
    qoutCerr = new QOutStream (std::cerr, ui->coutBox, QColor(180,0,0));
    qoutCout = new QOutStream (std::cout, ui->coutBox, QColor(0,0,0));

    // set default video paths
    ui->plainTextEditVieoFile->appendPlainText(QString(DEFAULT_VIDEO_PATH));
    ui->plainTextEditVideoOutput->appendPlainText(QString(DefaultOutVideoText.c_str()));

    // set default input method
    if (cvmanager.defaultInputType == evg::SrcVideo::CAMERA)
        ui->buttonCamera->setChecked(true);
    else if (cvmanager.defaultInputType == evg::SrcVideo::FILE)
        ui->buttonFile->setChecked(true);
    else
        assert(false);

    // set default detector and tracker
    setDetectorCheckbox(cvmanager.defaultDetectorType);
    setTrackerCheckbox(cvmanager.defaultTrackerType);

    // set default retection time
    double maxPeriodExp = cvmanager.maxPeriodExp;
    double minPeriodExp = cvmanager.minPeriodExp;
    double position = (cvmanager.defaultPeriodExp - minPeriodExp)
                    * 100. / (maxPeriodExp - minPeriodExp);
    ui->redetectionTime->setValue(position);

    connect(this, SIGNAL(updateScreenSignal(const cv::Mat&)),
            this, SLOT(updateScreen(const cv::Mat&)));

}


QtMainwindow::~QtMainwindow()
{
    delete qoutCerr;
    delete qoutCout;
    delete ui;
}


void QtMainwindow::setDetectorCheckbox (const DetectorType _type)
{
    // check the necessary checkbox
    if      (_type == OPENCV_DETECTOR)
        ui->buttonDetectorOpencv->setChecked(true);
    else if (_type == KYLE_DETECTOR)
        ui->buttonDetectorKyle->setChecked(true);
    else
        assert(false);
}


void QtMainwindow::setTrackerCheckbox (const TrackerType _type)
{
    // check the necessary checkbox
    if      (_type == OPTICAL_FLOW_TRACKER)
        ui->buttonTrackerOpticalFlow->setChecked(true);
    else if (_type == CAMSHIFT_TRACKER)
        ui->buttonTrackerCamShift->setChecked(true);
    else if (_type == COMPRESSIVE_TRACKER)
        ui->buttonTrackerCompressive->setChecked(true);
    else if (_type == KYLE_TRACKER)
        ui->buttonTrackerKyle->setChecked(true);
    else
        assert(false);
}


void QtMainwindow::syncScreen()
{
    Mat tempCopy;
    while(syncScreenIsOn == true)
    {
        // Acquire lock on the queue
        boost::unique_lock<boost::mutex> lock(cvmanager.frame_mutex);
        cvmanager.frame_notifier.wait(lock);

        tempCopy = cvmanager.showFrame.clone();
        emit updateScreenSignal(tempCopy);
    }
}


// put image onto the screen
void QtMainwindow::updateScreen (const cv::Mat& image)
{
    Size screenSize = Size(ui->labelScreen->width(), ui->labelScreen->height());

    image_aux = image.clone();
    Size srcSize = image.size();
    double scale = min(double(screenSize.width) / srcSize.width,
                       double(screenSize.height) / srcSize.height);
    Size newSize;
    newSize.width = srcSize.width * scale;
    newSize.height = srcSize.height * scale;
    cv::resize (image_aux, image_aux, newSize);
    cvtColor (image_aux, image_aux, CV_BGR2RGB);
    unsigned int horMargin = (screenSize.width - newSize.width) / 2;
    unsigned int vertMargin = (screenSize.height - newSize.height) / 2;
    copyMakeBorder (image_aux, image_aux, vertMargin, vertMargin, horMargin, horMargin,
                    BORDER_CONSTANT, 0);

    QImage qt_image((uchar*)image_aux.data, image_aux.cols, image_aux.rows, image_aux.step,
                    QImage::Format_RGB888);

    ui->labelScreen->setPixmap(QPixmap::fromImage(qt_image));
}



// ---- high level CVmanager controls --- //


void QtMainwindow::initCVmanager()
{   
    cvmanager.start();

    syncScreenIsOn = true;
    syncThread = boost::thread(&QtMainwindow::syncScreen, this);
}

void QtMainwindow::resumeCVmanager()
{
    cvmanager.prepareLoop();
    cvmanager.runLoop();
    setDetectorCheckbox(cvmanager.detectorType);
    setTrackerCheckbox(cvmanager.trackerType);
}

void QtMainwindow::pauseCVmanager()
{
    cvmanager.stopLoop();
}

void QtMainwindow::stopCVmanager()
{
    syncScreenIsOn = false;
    syncThread.join();
    cout << "QtMainwindow::stopCVmanager(): syncThread is joined." << endl;

    cvmanager.stopLoop();
    cvmanager.finish();
}


// ----- events ----- //


void QtMainwindow::on_Stop_clicked()
{
    stopCVmanager();
}

void QtMainwindow::on_Start_clicked()
{
    // set input video
    evg::SrcVideo::Type resourceType = (ui->buttonCamera->isChecked() ? evg::SrcVideo::CAMERA : evg::SrcVideo::FILE);
    const string videoPath = ui->plainTextEditVieoFile->toPlainText().toUtf8().constData();
    cvmanager.input = evg::SrcVideo(resourceType, videoPath);

    // set output video
    // clear the default text, if nothing is set there instead
    if (ui->plainTextEditVideoOutput->toPlainText().toUtf8().constData() == DefaultOutVideoText)
        ui->plainTextEditVideoOutput->clear();
    cvmanager.outputPath = ui->plainTextEditVideoOutput->toPlainText().toUtf8().constData();

    initCVmanager();
    resumeCVmanager();
}

void QtMainwindow::on_buttonDetectorOpencv_clicked()
{
    pauseCVmanager();
    cvmanager.detectorType = OPENCV_DETECTOR;
    resumeCVmanager();
}

void QtMainwindow::on_buttonDetectorKyle_clicked()
{
    pauseCVmanager();
    cvmanager.detectorType = KYLE_DETECTOR;
    resumeCVmanager();
}

void QtMainwindow::on_buttonTrackerOpticalFlow_clicked()
{
    pauseCVmanager();
    cvmanager.trackerType = OPTICAL_FLOW_TRACKER;
    resumeCVmanager();
}

void QtMainwindow::on_buttonTrackerCamShift_clicked()
{
    pauseCVmanager();
    cvmanager.trackerType = CAMSHIFT_TRACKER;
    resumeCVmanager();
}

void QtMainwindow::on_buttonTrackerCompressive_clicked()
{
    pauseCVmanager();
    cvmanager.trackerType = COMPRESSIVE_TRACKER;
    resumeCVmanager();
}

void QtMainwindow::on_buttonTrackerKyle_clicked()
{
    pauseCVmanager();
    cvmanager.trackerType = KYLE_TRACKER;
    resumeCVmanager();
}

void QtMainwindow::on_redetectionTime_sliderMoved(int position)
{
    double maxPeriodExp = cvmanager.maxPeriodExp;
    double minPeriodExp = cvmanager.minPeriodExp;
    double periodExp = position / 100. * (maxPeriodExp - minPeriodExp) + minPeriodExp;
      //
    if (! cvmanager.setRedetectionPeriod(periodExp) )
        cerr << "Cannot set redetection period." << endl;
}
