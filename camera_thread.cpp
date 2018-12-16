#include "camera_thread.h"

CameraThread::CameraThread()
{
}
void CameraThread::run()
{
    if(!_camera_stream->isOpened()) {
        tools::displayCustomMessage("Camera error",
                                    "Cannot connect to camera. Camera thread ends");
        emit cameraDisconnected();
    }
    while(true)
    {
        if(_camera_stream->read(_single_frame)) {
            //_camera_stream->retrieve(_single_frame);
            _frames_buff->buffWrite(_single_frame);

            //detectFacesOnFrame();
            sendFrameToDisplay(_single_frame);
            _frame_cnt++;

            if(_frame_cnt >= 32) {
                _frame_cnt = 0;
                _end_time = QTime::currentTime();
                _elapsed_time = tools::timespaceInSeconds(_start_time, _end_time);
                double current_fps_value = 32.0 / _elapsed_time;
                emit currentFpsValue(current_fps_value);

                _start_time = _end_time;
            }
        }
        if(_end_requst){
            break;
        }
    }

}
void CameraThread::setFrameBuffer(std::shared_ptr<FrameBuffer>  frameBuffer)
{
    _frames_buff = frameBuffer;
}
void CameraThread::sendFrameToDisplay(cv::Mat& frame)
{
    QPixmap pixmap;
    if (frame.channels()== 3){
            cv::cvtColor(frame, frame, CV_BGR2RGB);
            pixmap = QPixmap::fromImage(QImage(static_cast<const unsigned char*>(frame.data),
                             frame.cols,frame.rows,QImage::Format_RGB888));
    }
    else
    {
            pixmap = QPixmap::fromImage(QImage(static_cast<const unsigned char*>(frame.data),
                             frame.cols,frame.rows,QImage::Format_Indexed8));
    }
    emit drawPixmap(pixmap);
}
void CameraThread::detectFacesOnFrame()
{
    cv::cuda::GpuMat image_gpu(_single_frame);

    cv::cuda::GpuMat objbuf;
    //_cascade_gpu->detectMultiScale(image_gpu, objbuf);
    std::vector<cv::Rect> detected_faces;
    //_cascade_gpu->convert(objbuf, detected_faces);
    //for(int i = 0; i < detections_num; ++i)
       cv::rectangle(_single_frame, detected_faces[0], cv::Scalar(255));
}
void CameraThread::end() {
    _end_requst = true;
}
