#ifndef FACE_DETECTOR_THREAD_H
#define FACE_DETECTOR_THREAD_H

#include <QThread>
#include <QTime>
#include <memory>
#include "framebuffer.h"

class FaceDetectorThread : public QThread
{
    Q_OBJECT

public:
    FaceDetectorThread();

private:
    bool endRequst = false;
    QTime startTime = QTime::currentTime();
    QTime endTime = QTime::currentTime();
    double elapsedTime = 0.0;
    std::shared_ptr<FrameBuffer> framesBuff;
    QMutex lockMutex, readMutex;
    uint8_t bufferSize = 60;
};

#endif // FACE_DETECTOR_THREAD_H
