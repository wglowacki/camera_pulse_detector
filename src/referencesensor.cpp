#include "referencesensor.h"
#include <QDebug>
#include <QRegExp>
#include <iomanip>
#include <QMutex>

ReferenceSensorThread::ReferenceSensorThread()
{
    initSerialPortData();
}

void ReferenceSensorThread::run()
{
    QMutex mtx;
    while(true)
    {
        if(port.canReadLine()) {
            QString read = port.readLine().simplified();
            if(saveFlag) {
                mtx.lock();
                auto t = std::time(nullptr);
                auto tm = *std::localtime(&t);
                std::ostringstream oss;
                oss << std::put_time(&tm, "%M-%S");
                std::string timeStamp = oss.str();
                saveFile << timeStamp << " " << read.toStdString() << "\n";
                mtx.unlock();
            }
            publishData(read);
        }
        msleep(200);
    }

}

void ReferenceSensorThread::initSerialPortData()
{
    port.setPortName("/dev/ttyACM0");
    port.setBaudRate(9600);
    port.setDataBits(QSerialPort::Data8);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    port.setFlowControl(QSerialPort::NoFlowControl);
}

void ReferenceSensorThread::openSerialPort()
{
    if (!port.open(QSerialPort::ReadWrite)) {
        qDebug() << "Error opening serial port: "
                  << port.errorString();
    } else {
        qDebug() << "Sensor start";
    }
}

void ReferenceSensorThread::startSaveStatus(bool sf, std::string fn)
{
    QMutex mtx;
    mtx.lock();
    saveFlag = sf;
    if(saveFlag) {
        if(saveFile.is_open()) {
            saveFile.close();
        }
        std::string name = "sensor_" + fn + ".txt";
        saveFile.open(name);
        saveFlag = true;
    }
    else {
        saveFlag = false;
        saveFile.close();
    }
    mtx.unlock();
}

void ReferenceSensorThread::closeSerialPort()
{
    port.close();
    saveFlag = false;
    saveFile.close();
}
void ReferenceSensorThread::end()
{
    closeSerialPort();
}
