#ifndef ALGORITHM_THREAD_H
#define ALGORITHM_THREAD_H

#include <QThread>
#include <QDebug>

#include <memory>
#include <opencv2/opencv.hpp>
#include "framebuffer.h"
#include "functionsandtools.h"

class AlgorithmThread : public QThread
{
    Q_OBJECT

public slots:
    void end();
signals:
//    void calculate();

public:
    AlgorithmThread(QObject* parent = nullptr);
    void setForeheadBuffer(
            QVector<std::shared_ptr<FrameBuffer>> &frameBuffer);
    void run() override;

private:
    QVector<double> calcRoiMeans();
    QVector<double> calcLinspaceTimes(int vectSize);
    QVector<double> calcInterpMeans(
            const QVector<double>& evenTimes,
            QVector<double>& means
    );
    int minSize = 30;
    QVector<std::shared_ptr<FrameBuffer>> foreheadBuff;


};
#endif // ALGORITHM_THREAD_H
