#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QPixmap>
#include <QTextStream>
#include <QMessageBox>
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include "camera_thread.h"
#include "functions_and_tools.h"
#include "frame_buffer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setWindowStylesheet();
    void defineSignals();
    ~MainWindow();
public slots:
    void startCameraThread();
    void currentFpsValue(double value);
    void cameraDisconnected();
    void drawPixmap(QPixmap image);

private:
    Ui::MainWindow *ui;
    CameraThread _camera_thread;
//    FaceDetectorThread _face_detector_thread;
//    AlgorithmThread _algorithm_thread;
//    ReferenceSensorThread _reference_sensor_thread;
    std::vector<std::shared_ptr<FrameBuffer>> _face_detected_image_buffer;
    uint8_t _bufferSize = 30;
};

#endif // MAINWINDOW_H
