#ifndef FACE_DETECTOR_THREAD_H
#define FACE_DETECTOR_THREAD_H

#include <QThread>
#include <QTime>
#include <memory>
#include <mutex>
#include "frame_buffer.h"

class FaceDetectorThread : public QThread
{
    Q_OBJECT
private:
    bool _end_requst = false;
    QTime _start_time = QTime::currentTime();
    QTime _end_time = QTime::currentTime();
    double _elapsed_time = 0.0;
    std::shared_ptr<FrameBuffer> _frames_buff;
    std::mutex _lock_mutex, _read_mutex;
    uint8_t _buffer_size = 60;
public:
    FaceDetectorThread();
};

#endif // FACE_DETECTOR_THREAD_H
