#include "camerathread.h"
#include <QPixmap>
#include <limits>

CameraThread::CameraThread()
{
    cameraStream->set(CV_CAP_PROP_FPS, 15);
    cameraStream->set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cameraStream->set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    videoStream->set(CV_CAP_PROP_FPS, 15);
    videoStream->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    videoStream->set(CV_CAP_PROP_FRAME_HEIGHT, 720);

}
void CameraThread::run()
{
    if(!cameraStream->isOpened()) {
        tools::dispQMsg("Camera error",
            "Cannot connect to camera. Camera thread ends");
        emit cameraDisconnected();
    }
    endRequest = false;
    while(true)
    {
        if (videoStream->isOpened()) {
            auto captTs = std::chrono::system_clock::now();
            usFrameTs = (captTs - startTs);
            if(videoStream->grab())
                videoStream->read(singleFrame);
            if (singleFrame.empty()) break;
        } else if(cameraStream->grab()) {
            auto captTs = std::chrono::system_clock::now();
            usFrameTs = (captTs - startTs);
            cameraStream->retrieve(singleFrame);
        } else {
            continue;
        }
            detectFacesOnFrame();
            sendFrameToDisplay(singleFrame);

            ++frameCnt;

            if(frameCnt >= 32) {
                frameCnt = 0;
                endTime = Time::now();
                timeDiff = endTime - startTime;
                elapsedTime = timeDiff.count();
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
    qDebug() << "File name" << fn;
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


void CameraThread::setImageReceivedFlag(bool& sharedFlag)
{
    flagReceivedNewImage = sharedFlag;
}

void CameraThread::sendFrameToDisplay(cv::Mat& frame)
{

    cv::resize(frame, frame, cv::Size(640, 480), 0, 0, cv::INTER_AREA);
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
    if(!lockForeheadState) {
        cv::cuda::GpuMat imageGpu(gray);
        cv::cuda::GpuMat objBuf;
        cascadeGpu->detectMultiScale(imageGpu, objBuf);
        cascadeGpu->convert(objBuf, detectedFaces);
    }

    uint8_t i = 0;
    for (auto const& face: detectedFaces)
    {
        auto faceGray = gray(face);
        if(face.height > 0 && face.width > 0) {
            auto fh = extractForehead(face);
            cv::Mat matFace = cv::Mat(singleFrame, face);
            cv::Mat matFh = cv::Mat(singleFrame, fh);

            cv::Mat channels[3];
            cv::split(matFh, channels);
            channels[0] = cv::Mat::zeros(
                        matFh.rows, matFh.cols, CV_8UC1);
            channels[2] = cv::Mat::zeros(
                        matFh.rows, matFh.cols, CV_8UC1);
            cv::merge(channels,3,matFh);

            faceBuff.at(i)->buffWrite(
                        matFace, usFrameTs.count());
            foreheadBuff.at(i)->buffWrite(
                        channels[1], usFrameTs.count());
            flagReceivedNewImage = true;

            cv::rectangle(singleFrame, fh, cv::Scalar(255));
        }
        cv::rectangle(singleFrame, face, cv::Scalar(255));
        ++i;
    }
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

void CameraThread::lockForehead(bool state)
{
    lockForeheadState = state;
}

void CameraThread::end() {
    endRequest = true;
}
