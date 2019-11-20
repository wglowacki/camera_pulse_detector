#pragma once

#include <QSerialPort>
#include <QThread>
#include <fstream>

class ReferenceSensorThread : public QThread
{
Q_OBJECT
public slots:
    void end();
signals:
    void publishData(QString data);
public:
    ReferenceSensorThread();
    void startSaveStatus(bool saveFlag, std::string fn);
    void run() override;
    void openSerialPort();
    void closeSerialPort();

private:
    bool saveFlag = false;
    std::ofstream saveFile;
    void initSerialPortData();
    QSerialPort port;
};
