#include "camerathread.h"
#include <QPixmap>

CameraThread::CameraThread()
{
}
void CameraThread::run()
{
    if(!cameraStream->isOpened()) {
        tools::dispQMsg("Camera error",
            "Cannot connect to camera. Camera thread ends");
        emit cameraDisconnected();
    }
    while(true)
    {
        if (videoStream->isOpened()) {
            videoStream->read(singleFrame);
            if (singleFrame.empty()) break;
        } else if(cameraStream->grab()) {
            cameraStream->retrieve(singleFrame);
        } else {
            continue;
        }
            detectFacesOnFrame();
            sendFrameToDisplay(singleFrame);
            frameCnt++;

            if(frameCnt >= 32) {
                frameCnt = 0;
                endTime = QTime::currentTime();
                elapsedTime = tools::timeInSec(startTime, endTime);
                double fps = 32.0 / elapsedTime;
                emit currentFpsValue(fps);
                startTime = endTime;
            }
        if(endRequest) {
            break;
        }
    }
}

void CameraThread::readFromFile(QString fn) {
    videoStream->open(fn.toStdString());
    qDebug() << fn;

}
void CameraThread::setFrameBuffer(QVector<std::shared_ptr<FrameBuffer> > &frameBuffer)
{
    framesBuff = frameBuffer;
}

void CameraThread::sendFrameToDisplay(cv::Mat& frame)
{
    QPixmap pixmap;
    if (frame.channels()== 3) {
            cv::cvtColor(frame, frame, CV_BGR2RGB);
            pixmap = QPixmap::fromImage(QImage(static_cast<const unsigned char*>(frame.data),
                             frame.cols,frame.rows,QImage::Format_RGB888));
    } else {
            pixmap = QPixmap::fromImage(QImage(static_cast<const unsigned char*>(frame.data),
                             frame.cols,frame.rows,QImage::Format_Indexed8));
    }
    emit drawPixmap(pixmap);
}
void CameraThread::detectFacesOnFrame()
{
    cv::Mat gray;
    cv::cvtColor(singleFrame, gray, cv::COLOR_RGB2GRAY);
    cv::cuda::GpuMat imageGpu(gray);
    cv::cuda::GpuMat objBuf;
    cascadeGpu->detectMultiScale(imageGpu, objBuf);
    std::vector<cv::Rect> detectedFaces;
    cascadeGpu->convert(objBuf, detectedFaces);
    uint8_t i = 0;

    for (auto const& face: detectedFaces)
    {
        cv::rectangle(singleFrame, face, cv::Scalar(255));
        auto roi = gray(face);
        framesBuff.at(i)->buffWrite(roi);
    }
//    _single_frame
//    cv::cvtColor(gray, _single_frame, cv::COLOR_GRAY2RGB);
}
void CameraThread::end() {
    endRequest = true;
}
