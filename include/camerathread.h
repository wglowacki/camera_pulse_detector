#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

#include <QThread>
#include <QTime>
#include <QDebug>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/cudaimgproc.hpp>
#include "functionsandtools.h"
#include "framebuffer.h"

class CameraThread : public QThread
{
    Q_OBJECT

public slots:
    void end();
signals:
    void drawPixmap(QPixmap pixmap);
    void cameraDisconnected();
    void currentFpsValue(double value);

public:
    CameraThread();
    void setFrameBuffer(QVector<std::shared_ptr<FrameBuffer>>& frameBuffer);
    void sendFrameToDisplay(cv::Mat& frame);
    void detectFacesOnFrame();
    bool isCameraAvailable();
    cv::Mat readFrame(cv::Mat& frame);
    void readFromFile(QString fn);
    bool writeFramesBuffer(cv::Mat& frame);
    bool readFramesBuffer(cv::Mat& frame);
    void run() override;

private:
    bool endRequest = false;
    QTime startTime = QTime::currentTime();
    QTime endTime = QTime::currentTime();
    double elapsedTime = 0.0;
    std::unique_ptr<cv::VideoCapture> cameraStream = std::make_unique<cv::VideoCapture>(0);
    std::unique_ptr<cv::VideoCapture> videoStream = std::make_unique<cv::VideoCapture>();
    QVector<std::shared_ptr<FrameBuffer>> framesBuff;
    cv::Mat singleFrame;
    uint8_t frameCnt = 0;
    uint8_t bufferSize = 60;
    int droppedFramesCounter = 0;
    QMutex lockMutex, readMutex;
    std::string pathToHaarDetector = "/home/wojtas/magisterka/camera_pulse_detector/classifiers/haarcascade_frontalface_alt.xml";
    cv::Ptr<cv::cuda::CascadeClassifier> cascadeGpu = cv::cuda::CascadeClassifier::create(pathToHaarDetector);
};

#endif // CAMERA_THREAD_H
