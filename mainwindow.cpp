#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QPixmap>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowStylesheet();

    faceDetectedImageBuffer.push_back (
        std::make_shared<FrameBuffer>(bufferSize)
    );
    foreheadDetectedImageBuffer.push_back (
        std::make_shared<FrameBuffer>(bufferSize)
    );

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
    connect(ui->algorithmButton, &QPushButton::clicked,
        this, &MainWindow::startPPMAlg);
    connect(ui->openMovieButton, &QPushButton::clicked,
        this, &MainWindow::openMovie, Qt::QueuedConnection);
    connect(&cameraThread, &CameraThread::cameraDisconnected,
        this, &MainWindow::cameraDisconnected, Qt::QueuedConnection);
    connect(&cameraThread, &CameraThread::currentFpsValue,
        this, &MainWindow::currFpsVal, Qt::QueuedConnection);
    connect(&cameraThread, &CameraThread::drawPixmap,
        this, &MainWindow::drawPixmap, Qt::QueuedConnection);
    connect(&cameraThread, &CameraThread::drawDetection,
        this, &MainWindow::drawDetection, Qt::QueuedConnection);
    connect(&refSensorThread, &ReferenceSensorThread::publishData,
        this, &MainWindow::newSensorRead, Qt::QueuedConnection);
    connect(ui->referenceSensorEnableCheckbox, &QCheckBox::stateChanged,
        this, &MainWindow::changeReferenceSensStatus, Qt::QueuedConnection);
    connect(ui->saveVideoCheckbox, &QCheckBox::stateChanged,
        this, &MainWindow::startSaveStatus, Qt::QueuedConnection);
    connect(ui->readPylonCheckbox, &QCheckBox::stateChanged,
        &cameraThread, &CameraThread::pylonCameraRead, Qt::QueuedConnection);

    connect(this, &MainWindow::lockForehead,
        &cameraThread, &CameraThread::lockForehead, Qt::QueuedConnection);
    connect(this, &MainWindow::disconnnectCamera,
        &cameraThread, &CameraThread::end, Qt::QueuedConnection);
    connect(ui->spinFaceInertia, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            &cameraThread, &CameraThread::changeInertia);
    connect(ui->spinBufMax, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=]( const int &newValue ) { this->bufferSize = newValue; });
    connect(ui->spinBufMin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            &cameraThread, &CameraThread::changeMinBuffer);

    connect(&algorithmThread, &AlgorithmThread::bpsUpdate,
            [this](double value) {
        ui->labelBps->setText(QString::number(value, 'f', 2));
    });
}
void MainWindow::startCameraThread()
{
    ui->connectCameraButton->setChecked(false);
    ui->connectCameraButton->setText("Capturing frames");

    cameraThread.setFaceBuffer(faceDetectedImageBuffer);
    cameraThread.setForeheadBuffer(foreheadDetectedImageBuffer);
    cameraThread.setImageReceivedFlag(flagReceivedNewImage);
    cameraThread.setImageReceivedFlag(flagReceivedNewImage);

    if(!cameraThread.isRunning()) {
        qDebug() << "Starting camera thread";
        cameraThread.init();
        cameraThread.start();
    } else {
        tools::dispQMsg("Camera running",
            "Camera already capturing frames.");
    }
    //implement camera selection;
}

void MainWindow::startSaveStatus(int newStatus)
{
    std::string fileName = " ";
    bool saveStatus = newStatus>0 ? true : false;
    if(saveStatus) {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
        fileName = oss.str();
    }
    cameraThread.startSaveStatus(saveStatus, fileName);
    refSensorThread.startSaveStatus(saveStatus, fileName);
    algorithmThread.startSaveStatus(saveStatus, fileName);

}

void MainWindow::changeReferenceSensStatus(int newState)
{
    if(newState) {
        refSensorThread.openSerialPort();
        refSensorThread.start();
    } else {
        if(cameraThread.isRunning()) {
            refSensorThread.quit();
            refSensorThread.closeSerialPort();
        }
    }
}

void MainWindow::startPPMAlg()
{
    if(cameraThread.isRunning()) {
        ui->algorithmButton->setText("AlgorithmRunning");
        qDebug() << "Starting algorithm thread";
        algorithmThread.setForeheadBuffer(foreheadDetectedImageBuffer);
        algorithmThread.start();
    } else {
        tools::dispQMsg("Capture not running",
            "Start reading frames first");
    }
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
void MainWindow::drawDetection(QPixmap image)
{
   ui->algorithmArea->setPixmap(image);
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

void MainWindow::newSensorRead(QString data)
{
    ui->labelSensor->setText(data);
}

void MainWindow::keyPressEvent(QKeyEvent* key)
{
    if(key->text() == "q") {
        emit disconnnectCamera();
    } else if(key->text() == "l") {
        emit lockForehead(true);
    }  else if(key->text() == "u") {
        emit lockForehead(false);
    }
}

MainWindow::~MainWindow()
{
    if(cameraThread.isRunning())
        cameraThread.end();
    if(algorithmThread.isRunning())
        algorithmThread.end();
    if(refSensorThread.isRunning())
        refSensorThread.end();

    delete ui;
}
