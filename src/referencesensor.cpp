#include "referencesensor.h"
#include <QDebug>
#include <QRegExp>

ReferenceSensorThread::ReferenceSensorThread()
{
    initSerialPortData();
}

void ReferenceSensorThread::run()
{
    while(true)
    {
        if(port.canReadLine()) {
            int read = port.readLine().simplified().toInt();
            qDebug() << read;
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

void ReferenceSensorThread::closeSerialPort()
{
    port.close();
}
