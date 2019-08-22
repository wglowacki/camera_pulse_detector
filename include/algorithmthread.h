#ifndef ALGORITHM_THREAD_H
#define ALGORITHM_THREAD_H

#include <QThread>
#include <QDebug>

#include <memory>
#include <opencv2/opencv.hpp>

#include <gsl/gsl_complex.h>

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
    QVector<gsl_complex> calcFFT(const QVector<double>& vectMeans);
    QVector<double> calcComplexFftAngles(
            const QVector<gsl_complex>& fftraw
    );
    QVector<double> calcComplexFftAbs(
            const QVector<gsl_complex>& fftraw
    );
    int minSize = 30;
    QVector<std::shared_ptr<FrameBuffer>> foreheadBuff;


};
#endif // ALGORITHM_THREAD_H
