#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

#include <QThread>
#include <QTime>
#include <QPixmap>
#include <memory>
#include <mutex>
#include <opencv2/opencv.hpp>
#include "functions_and_tools.h"
#include "frame_buffer.h"

class CameraThread : public QThread
{
    Q_OBJECT
private:
    bool _end_requst = false;
    QTime _start_time = QTime::currentTime();
    QTime _end_time = QTime::currentTime();
    double _elapsed_time = 0.0;
    std::unique_ptr<cv::VideoCapture> _camera_stream = std::make_unique<cv::VideoCapture>(0);
    std::shared_ptr<FrameBuffer> _frames_buff;
    cv::Mat _single_frame;
    uint8_t _frame_cnt = 0;
    uint8_t _buffer_size = 60;
    int _dropped_frames_counter = 0;
    std::mutex _lock_mutex, _read_mutex;
public:
    CameraThread();
    void setFrameBuffer(std::shared_ptr<FrameBuffer> frameBuffer);
    void sendFrameToDisplay(cv::Mat& frame);
    bool isCameraAvailable();
    cv::Mat readFrame(cv::Mat& frame);
    bool writeFramesBuffer(cv::Mat& frame);
    bool readFramesBuffer(cv::Mat& frame);
    void run() override;
public slots:
    void end();
signals:
    void drawPixmap(QPixmap pixmap);
    void cameraDisconnected();
    void currentFpsValue(double value);
};

#endif // CAMERA_THREAD_H
