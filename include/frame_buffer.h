#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <QObject>
#include <QMutex>
#include <queue>
#include <memory>
#include <opencv2/opencv.hpp>

class FrameBuffer : public QObject
{
    Q_OBJECT
public:
    explicit FrameBuffer(uint8_t bufferSize, QObject *parent = nullptr);
    ~FrameBuffer() {}
    void clear();
    bool buffWrite(cv::Mat& image);
    bool buffRead(cv::Mat& image);
    uint8_t getBufferSize();
    uint8_t getElementsInBuffer();
signals:

    public slots :
        void setActive(bool active);
private:
    QMutex _buffer_mutex;
    int _drop_cnt;
    std::queue<cv::Mat> _buffer;
    bool _active;
    uint8_t _buffer_size;
};

#endif // FRAMEBUFFER_H
