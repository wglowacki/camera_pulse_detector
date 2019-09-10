#include "framebuffer.h"

FrameBuffer::FrameBuffer(uint8_t setBufSize, QObject *parent)
    : QObject(parent)
{
    dropCnt = 0;
    bufferSize = setBufSize;
    active = true;
}

void FrameBuffer::clear()
{
    bufferMutex.lock();
    dropCnt = 0;
    buffer.clear();
    bufferMutex.unlock();
}

bool FrameBuffer::buffWrite(const cv::Mat& image, double currT)
{
    bool success = false;
    bufferMutex.lock();
    if (active) {
        if (buffer.size() < bufferSize) {
            std::pair<cv::Mat, double> dataToBuff;
            dataToBuff.first = image;
            dataToBuff.second = currT;
            buffer.enqueue(dataToBuff);
            success = true;
        } else {
            buffer.dequeue();
            dropCnt++;
        }
    }
    bufferMutex.unlock();

    return success;
}

void FrameBuffer::setActive(bool active) {
    bufferMutex.lock();
    this->active = active;
    bufferMutex.unlock();
}

uint8_t FrameBuffer::getBufferSize() {
    return buffer.size();
}
