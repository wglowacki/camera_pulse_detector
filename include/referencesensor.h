#pragma once

#include <QSerialPort>
#include <QThread>

class ReferenceSensorThread : public QThread
{
Q_OBJECT
signals:
    void publishData(int data);
public:
    ReferenceSensorThread();
    void run() override;
    void openSerialPort();
    void closeSerialPort();

private:

    void initSerialPortData();
    QSerialPort port;
};
