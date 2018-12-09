#include "frame_buffer.h"

FrameBuffer::FrameBuffer(uint8_t _buffer_size, QObject *parent) : QObject(parent)
{
    _drop_cnt = 0;
    this->_buffer_size=_buffer_size;
    _active = true;
}

void FrameBuffer::clear()
{
    _buffer_mutex.lock();
    _drop_cnt = 0;
    _buffer = std::queue<cv::Mat>();
    _buffer_mutex.unlock();
}

bool FrameBuffer::buffWrite(cv::Mat& image)
{
    bool success = false;
    _buffer_mutex.lock();
    if (_active) {
        if (_buffer.size() < _buffer_size) {
            _buffer.push(image);
            success = true;
        } else {
            _drop_cnt++;
        }
    }
    _buffer_mutex.unlock();

    return success;
}

void FrameBuffer::setActive(bool active) {
    _buffer_mutex.lock();
    this->_active = active;
    _buffer_mutex.unlock();
}

uint8_t FrameBuffer::getBufferSize() {
    return _buffer_size;
}

bool FrameBuffer::buffRead(cv::Mat& image){
    bool success = false;
    _buffer_mutex.lock();
        if(_buffer.size() > 0) {
            image = _buffer.front();
            _buffer.pop();
            success = true;
        }
    _buffer_mutex.unlock();

    return success;
}
