#ifndef ALGORITHM_THREAD_H
#define ALGORITHM_THREAD_H

#include <QThread>
#include <memory>
#include <opencv2/opencv.hpp>
#include "functions_and_tools.h"

class AlgorithmThread : public QThread
{
    Q_OBJECT
private:
    std::shared_ptr<cv::Mat> _detected_faces;
public:
    AlgorithmThread(QObject* parent = nullptr);
    void run() override;
public slots:
//    void end();
signals:
//    void calculate();
};

#endif // ALGORITHM_THREAD_H
