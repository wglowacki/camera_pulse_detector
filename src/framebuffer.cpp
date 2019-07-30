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

bool FrameBuffer::buffWrite(cv::Mat& image)
{
    bool success = false;
    bufferMutex.lock();
    if (active) {
        if (buffer.size() < bufferSize) {
            buffer.push_back(image);
            success = true;
        } else {
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
    return bufferSize;
}

bool FrameBuffer::buffRead(cv::Mat& image){
    bool success = false;
    bufferMutex.lock();
        if(buffer.size() > 0) {
            image = buffer.front();
            buffer.pop_front();
            success = true;
        }
    bufferMutex.unlock();

    return success;
}
