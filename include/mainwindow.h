#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <memory>
#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <opencv2/opencv.hpp>
#include "camerathread.h"
#include "algorithmthread.h"
#include "functionsandtools.h"
#include "framebuffer.h"
#include "referencesensor.h"

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
    void changeReferenceSensStatus(int newState);
    void startPPMAlg();
    void currFpsVal(double value);
    void cameraDisconnected();
    void drawPixmap(QPixmap image);
    void newSensorRead(int data);
    void setWindowStylesheet();
    void defineSignals();
    void openMovie();

signals:
    void lockForehead(bool state);
    void disconnnectCamera();
protected:
    void keyPressEvent(QKeyEvent* key);

private:
    Ui::MainWindow *ui;
    CameraThread cameraThread;
//    FaceDetectorThread _face_detector_thread;
    AlgorithmThread algorithmThread;
    ReferenceSensorThread refSensorThread;
    QVector<std::shared_ptr<FrameBuffer>>
        faceDetectedImageBuffer;
    QVector<std::shared_ptr<FrameBuffer>>
        foreheadDetectedImageBuffer;
    bool flagReceivedNewImage = false;
    uint8_t bufferSize = 240;
};

#endif // MAINWINDOW_H
