#ifndef ALGORITHM_THREAD_H
#define ALGORITHM_THREAD_H

#include <QThread>
#include <memory>
#include <opencv2/opencv.hpp>
#include "functionsandtools.h"

class AlgorithmThread : public QThread
{
    Q_OBJECT

public slots:
//    void end();
signals:
//    void calculate();

public:
    AlgorithmThread(QObject* parent = nullptr);
    void run() override;

private:
    std::shared_ptr<cv::Mat> detectedFaces;

};
#endif // ALGORITHM_THREAD_H
