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

void CameraThread::readFromFile(QString fn)
{
    videoStream->open(fn.toStdString());
    qDebug() << fn;
}

void CameraThread::setFaceBuffer(
        QVector<std::shared_ptr<FrameBuffer>> &frameBuffer)
{
    faceBuff = frameBuffer;
}

void CameraThread::setForeheadBuffer(
        QVector<std::shared_ptr<FrameBuffer>> &frameBuffer)
{
    foreheadBuff = frameBuffer;
}

void CameraThread::sendFrameToDisplay(cv::Mat& frame)
{
    QPixmap pixmap;
    if (frame.channels()== 3) {
            cv::cvtColor(frame, frame, CV_BGR2RGB);
            pixmap = QPixmap::fromImage(QImage(
                static_cast<const unsigned char*>(frame.data),
                frame.cols,frame.rows,QImage::Format_RGB888));
    } else {
            pixmap = QPixmap::fromImage(QImage(
                static_cast<const unsigned char*>(frame.data),
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
        auto faceGray = gray(face);
        if(face.height > 0 && face.width > 0) {
            auto fh = extractForehead(face);
            cv::Mat matFace = cv::Mat(singleFrame, face);
            cv::Mat matFh = cv::Mat(singleFrame, fh);
            auto captTs = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed = captTs - startTs;
            faceBuff.at(i)->buffWrite(
                        matFace, elapsed.count());
            foreheadBuff.at(i)->buffWrite(
                        matFh, elapsed.count());
            cv::rectangle(singleFrame, fh, cv::Scalar(255));
        }
        cv::rectangle(singleFrame, face, cv::Scalar(255));
    }
//    _single_frame
//    cv::cvtColor(gray, _single_frame, cv::COLOR_GRAY2RGB);
}

cv::Rect CameraThread::extractForehead(const cv::Rect& face)
{
    cv::Rect fh;
    fh.x = face.x + face.width*foreheadPos.x
            - (face.width * foreheadPos.w / 2.0);
    fh.y = face.y + face.height * foreheadPos.y
            - (face.height * foreheadPos.h / 2.0);
    fh.width = static_cast<int>(face.width * foreheadPos.w);
    fh.height = static_cast<int>(face.height * foreheadPos.h);

    return fh;
}

void CameraThread::end() {
    endRequest = true;
}
