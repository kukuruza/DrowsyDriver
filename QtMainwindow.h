#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core/core.hpp>
#include <boost/thread/thread.hpp>
#include <CVmanager.h>
#include <QElapsedTimer>
#include "QtOutStream.h"

namespace Ui {
class MainWindow;
}



class QtMainwindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit QtMainwindow(QWidget *parent = 0);
    ~QtMainwindow();

// framework for updating screen
signals:
    void updateScreenSignal(const cv::Mat& image);
private slots:
    void updateScreen(const cv::Mat& image);
private:
    void syncScreen();
    bool syncScreenIsOn;
    boost::thread syncThread;
    cv::Mat image_aux;

private slots:
    void on_Stop_clicked();
    void on_Start_clicked();
    void on_buttonDetectorOpencv_clicked();
    void on_buttonDetectorKyle_clicked();
    void on_buttonTrackerOpticalFlow_clicked();
    void on_buttonTrackerCamShift_clicked();
    void on_buttonTrackerCompressive_clicked();
    void on_buttonTrackerKyle_clicked();
    void on_redetectionTime_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    CVmanager cvmanager;

    // set the checkboxes (used by button events)
    void setDetectorCheckbox (const DetectorType type);
    void setTrackerCheckbox (const TrackerType type);

    // operations on CVmanager (used by all the button events)
    void initCVmanager();
    void pauseCVmanager();
    void resumeCVmanager();
    void stopCVmanager();

    // dummy text for video output
    const std::string DefaultOutVideoText;
    // redirection of std::cout and std::cerr to a box in the form
    QOutStream *qoutCout, *qoutCerr;

};

#endif // MAINWINDOW_H
