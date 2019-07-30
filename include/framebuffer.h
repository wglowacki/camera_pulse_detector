#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <QObject>
#include <QMutex>
#include <QQueue>
#include <opencv2/opencv.hpp>

class FrameBuffer : public QObject
{
    Q_OBJECT

public slots :
        void setActive(bool active);
public:
    explicit FrameBuffer(uint8_t bufferSize, QObject *parent = nullptr);
    ~FrameBuffer() {}
    void clear();
    bool buffWrite(cv::Mat& image);
    bool buffRead(cv::Mat& image);
    uint8_t getBufferSize();
    uint8_t getElementsInBuffer();

private:
    QMutex bufferMutex;
    int dropCnt;
    QQueue<cv::Mat> buffer;
    bool active;
    uint8_t bufferSize;
};

#endif // FRAMEBUFFER_H
