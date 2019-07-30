#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QPixmap>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowStylesheet();

    faceDetectedImageBuffer.push_back (
        std::make_shared<FrameBuffer>(bufferSize)
    );
    //faceDetectedImageBuffer.begin()->setActive(true);

    cameraThread.setFrameBuffer(faceDetectedImageBuffer);
//    _algorithm_thread.setFrameBuffer(faceDetectedImageBuffer);
    defineSignals();
}

void MainWindow::setWindowStylesheet()
{
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists()) {
        tools::dispQMsg("Stylesheet error",
            "Cannot open stylesheet qss file."
            "Window displayed in deafult mode");
        return;
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}
void MainWindow::defineSignals()
{
    connect(ui->connectCameraButton, &QPushButton::clicked,
        this, &MainWindow::startCameraThread, Qt::QueuedConnection);
   // connect(ui->algButton, &QPushButton::clicked,
   //     this, &MainWindow::startPPMAlg);
    connect(ui->openMovieButton, &QPushButton::clicked,
        this, &MainWindow::openMovie, Qt::QueuedConnection);
    connect(&cameraThread, &CameraThread::cameraDisconnected,
        this, &MainWindow::cameraDisconnected, Qt::QueuedConnection);
    connect(&cameraThread, &CameraThread::currentFpsValue,
        this, &MainWindow::currFpsVal, Qt::QueuedConnection);
    connect(&cameraThread, &CameraThread::drawPixmap,
        this, &MainWindow::drawPixmap, Qt::QueuedConnection);
}
void MainWindow::startCameraThread()
{
    ui->connectCameraButton->setChecked(false);
    ui->connectCameraButton->setText("Capturing frames");
    if(cameraThread.isRunning()) {
        tools::dispQMsg("Camera running",
            "Camera already capturing frames.");
    }
    //implement camera selection;
    cameraThread.start();
}
void MainWindow::cameraDisconnected()
{
    if(cameraThread.isRunning())
        cameraThread.end();
    while(cameraThread.isRunning())
    { }
}
void MainWindow::currFpsVal(double value)
{
    ui->fpsValueLabel->setText(QString::number(value));
}
void MainWindow::drawPixmap(QPixmap image)
{
    ui->imageDisplay->setPixmap(image);
}

void MainWindow::openMovie() {
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open Video"), QStandardPaths::displayName(
        QStandardPaths::DesktopLocation), tr(""));
    if(cameraThread.isRunning()) {
        cameraDisconnected();
    }
    cameraThread.readFromFile(fileName);
    cameraThread.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
