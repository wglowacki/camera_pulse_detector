#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <memory>
#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <opencv2/opencv.hpp>
#include "camerathread.h"
#include "functionsandtools.h"
#include "framebuffer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void startCameraThread();
    void currFpsVal(double value);
    void cameraDisconnected();
    void drawPixmap(QPixmap image);
    void setWindowStylesheet();
    void defineSignals();
    void openMovie();
private:
    Ui::MainWindow *ui;
    CameraThread cameraThread;
//    FaceDetectorThread _face_detector_thread;
//    AlgorithmThread _algorithm_thread;
//    ReferenceSensorThread _reference_sensor_thread;
    QVector<std::shared_ptr<FrameBuffer>> faceDetectedImageBuffer;
    uint8_t bufferSize = 30;
};

#endif // MAINWINDOW_H
