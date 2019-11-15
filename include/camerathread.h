#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

#include <QThread>
#include <chrono>
#include <QDebug>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/cudaimgproc.hpp>
#include "functionsandtools.h"
#include "framebuffer.h"

#define READ_PYLON
#ifdef READ_PYLON
#include "camerapylon.h"
#endif

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
    void setFaceBuffer(QVector< std::shared_ptr
                        <FrameBuffer> >& frameBuffer);
    void setForeheadBuffer(QVector< std::shared_ptr
                        <FrameBuffer> >& frameBuffer);
    void setImageReceivedFlag(bool& flagReceivedNewImage);
    void sendFrameToDisplay(cv::Mat& frame);
    void startSaveStatus(bool newState, std::string fn);
    void detectFacesOnFrame();
    bool isCameraAvailable();
    cv::Mat readFrame(cv::Mat& frame);
    void readFromFile(QString fn);
    bool writeFramesBuffer(cv::Mat& frame);
    bool readFramesBuffer(cv::Mat& frame);
    void lockForehead(bool state);
    void run() override;

private:
    bool endRequest = false;
    bool lockForeheadState = false;
    typedef std::chrono::high_resolution_clock Time;
    std::chrono::time_point<Time>  startTime = Time::now();
    std::chrono::time_point<Time>  endTime = Time::now();
    std::chrono::duration<double> timeDiff;
    std::chrono::time_point<std::chrono::system_clock>
        startTs = std::chrono::system_clock::now();
    double elapsedTime = 0.0;
    std::chrono::duration<double> usFrameTs;
    double grabTs = 0.0;
    uint64_t pylonTs = 0.0;
    bool saveStatus = false;
    cv::VideoWriter savedVideo;
#ifndef READ_PYLON
    std::unique_ptr<cv::VideoCapture> cameraStream
        = std::make_unique<cv::VideoCapture>(0);
#else
    CameraPylon pylonCamera;
#endif
    std::unique_ptr<cv::VideoCapture> videoStream
        = std::make_unique<cv::VideoCapture>();

    QVector<std::shared_ptr<FrameBuffer>> faceBuff;
    QVector<std::shared_ptr<FrameBuffer>> foreheadBuff;
    bool flagReceivedNewImage = false;
    QTime captTime;
    std::vector<cv::Rect> detectedFaces;
    QVector<QVector
        <std::shared_ptr<FrameBuffer>>> vectOfAllFace;
    QVector<QVector
        <std::shared_ptr<FrameBuffer>> > vectOfAllForehead;
    QVector<QVector<QTime>> vectOfAllTimestamps;

    struct ForeheadPos {
        double x = 0.5;
        double y = 0.14;
        double w = 0.42;
        double h = 0.20;
    } foreheadPos;

    cv::Mat singleFrame;
    uint8_t frameCnt = 0;
    uint8_t bufferSize = 60;
    int droppedFramesCounter = 0;
    QMutex lockMutex, readMutex;
    std::string pathToHaarDetector
        = "/home/agh/camera_pulse_detector/classifiers/haarcascade_frontalface_alt.xml";
    cv::Ptr<cv::cuda::CascadeClassifier> cascadeGpu
        = cv::cuda::CascadeClassifier::create(pathToHaarDetector);

    cv::Rect extractForehead(const cv::Rect& face);
};

#endif // CAMERA_THREAD_H
