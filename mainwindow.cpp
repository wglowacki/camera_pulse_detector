#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowStylesheet();

    _frame_buffer = std::make_shared<FrameBuffer>(_bufferSize);
    _face_detected_image_buffer = std::make_shared<FrameBuffer>(_bufferSize);
    _face_detected_image_buffer->setActive(false);

//    _face_detector_thread.setFrameBuffer(_frame_buffer);
    _camera_thread.setFrameBuffer(_frame_buffer);

//    _algorithm_thread.setFrameBuffer(_face_detected_image_buffer);
//    _face_detector_thread.addFrameBuffer(_face_detected_image_buffer);
    defineSignals();
}

void MainWindow::setWindowStylesheet()
{
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists()) {
        tools::displayCustomMessage("Stylesheet error",
                                   "Cannot open stylesheet qss file. Window displayed in deafult mode");
        return;
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}
void MainWindow::defineSignals()
{
    connect(ui->connectCameraButton, SIGNAL(clicked(bool)), this, SLOT(startCameraThread()), Qt::QueuedConnection);
    connect(ui->startAlgorithmButton, SIGNAL(clicked(bool)), this, SLOT(startPulseMeasurementAlgorithm()), Qt::QueuedConnection);
    connect(ui->stopAlgorithmButton, SIGNAL(clicked(bool)), this, SLOT(stopPulseMeasurementAlgorithm()), Qt::QueuedConnection);
    connect(&_camera_thread, SIGNAL(cameraDisconnected()), this, SLOT(cameraDisconnected()), Qt::QueuedConnection);
    connect(&_camera_thread, SIGNAL(currentFpsValue(double)), this, SLOT(currentFpsValue(double)), Qt::QueuedConnection);
    connect(&_camera_thread, SIGNAL(drawPixmap(QPixmap)), this, SLOT(drawPixmap(QPixmap)), Qt::QueuedConnection);
}
void MainWindow::startCameraThread()
{
    ui->connectCameraButton->setChecked(false);
    ui->connectCameraButton->setText("Capturing frames");
    if(_camera_thread.isRunning()) {
        tools::displayCustomMessage("Camera running", "Camera already capturing frames.");
    }
    //implement camera selection;
    _camera_thread.start();
}
void MainWindow::cameraDisconnected()
{
    if(_camera_thread.isRunning())
        _camera_thread.end();
    while(_camera_thread.isRunning())
    {}
}
void MainWindow::currentFpsValue(double value)
{
    ui->fpsValueLabel->setText(QString::number(value));
}
void MainWindow::drawPixmap(QPixmap image)
{
ui->imageDisplay->setPixmap(image);
}
MainWindow::~MainWindow()
{
    delete ui;
}
