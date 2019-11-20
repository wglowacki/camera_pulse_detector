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
#include "camerapylon.h"

class CameraThread : public QThread
{
    Q_OBJECT

public slots:
    void end();
signals:
    void drawPixmap(QPixmap pixmap);
    void drawDetection(QPixmap pixmap);
    void cameraDisconnected();
    void currentFpsValue(double value);

public:
    CameraThread() {}
    void init();
    void setFaceBuffer(QVector< std::shared_ptr
                        <FrameBuffer> >& frameBuffer);
    void setForeheadBuffer(QVector< std::shared_ptr
                        <FrameBuffer> >& frameBuffer);
    void setImageReceivedFlag(bool& flagReceivedNewImage);
    void sendFrameToDisplay(cv::Mat& frame, bool wholeFrame);
    void startSaveStatus(bool newState, std::string fn);
    void pylonCameraRead(int newState);
    void detectFacesOnFrame();
    bool isCameraAvailable();
    void changeInertia(int newValue);
    cv::Mat readFrame(cv::Mat& frame);
    void readFromFile(QString fn);
    bool writeFramesBuffer(cv::Mat& frame);
    bool readFramesBuffer(cv::Mat& frame);
    void lockForehead(bool state);
    void changeMinBuffer(int newValue);
    void run() override;

private:
    struct CaptureProperties {
        int width  = 640;
        int height = 480;
        int fps    =  20;
    };
    bool endRequest = false;
    bool lockForeheadState = false;
    cv::Rect prevFace = {0,0,0,0};
    typedef std::chrono::high_resolution_clock Time;
    std::chrono::time_point<Time>  startTime, endTime;
    std::chrono::duration<double> timeDiff;
    std::chrono::time_point<std::chrono::system_clock> startTs;
    QTime movieFps;
    bool readBaslerCamera = false;
    double elapsedTime = 0.0;
    std::chrono::duration<double> usFrameTs;
    double grabTs = 0.0;
    uint64_t pylonTs = 0.0;
    bool saveStatus = false;
    cv::VideoWriter savedVideo;
    int saveImageCounter = 0;
    std::string saveImageDir = "";
    std::shared_ptr<cv::VideoCapture> cameraStream;
    std::shared_ptr<CameraPylon> pylonCamera;

    std::shared_ptr<cv::VideoCapture> videoStream;

    QVector<std::shared_ptr<FrameBuffer>> faceBuff;
    QVector<std::shared_ptr<FrameBuffer>> foreheadBuff;
    bool flagReceivedNewImage = false;
    QTime captTime;

    int inertia = 15;
    std::vector<cv::Rect> detectedFaces;


    struct ForeheadPos {
        double x = 0.5;
        double y = 0.14;
        double w = 0.72;
        double h = 0.20;
    } foreheadPos;

    CaptureProperties cameraProp{640, 480, 20}, videoProp{640, 480, 20};
    cv::Mat singleFrame;
    uint8_t frameCnt = 0;
    uint8_t bufferSize = 60;
    int droppedFramesCounter = 0;
    QMutex lockMutex, readMutex;
    std::string pathToHaarDetector
        = "../classifiers/haarcascade_frontalface_alt.xml";
    cv::Ptr<cv::cuda::CascadeClassifier> cascadeGpu;

    bool checkIfShouldBeUpdated(const cv::Rect& face);
    cv::Rect extractForehead(const cv::Rect& face);
};

#endif // CAMERA_THREAD_H
