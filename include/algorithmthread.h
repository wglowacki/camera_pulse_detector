#ifndef ALGORITHM_THREAD_H
#define ALGORITHM_THREAD_H

#include <QThread>
#include <QDebug>

#include <memory>
#include <opencv2/opencv.hpp>

#include <gsl/gsl_complex.h>

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

#include "framebuffer.h"
#include "functionsandtools.h"
#include "matrixoperations.h"

class AlgorithmThread : public QThread
{
    Q_OBJECT

public slots:
    void end();
signals:
   void bpsUpdate(double value);

public:
    AlgorithmThread(QObject* parent = nullptr);
    void setForeheadBuffer(
            QVector<std::shared_ptr<FrameBuffer>> &frameBuffer);
    void run() override;

private:
    int minSize = 30;
    QMutex threadMutex;
    QVector<std::shared_ptr<FrameBuffer>> foreheadBuff;
    QVector<std::shared_ptr<FrameBuffer>> currentFHBuff;

    QVector<double> vectBPM;

    QVector<double> calcRoiMeans();
    QVector<double> calcLinspaceTimes(int vectSize, double startT, double endT);
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
    QVector<double> getDesiredFreqs(int size, double freq);
    QVector<int> trimFreqs(const QVector<double>& genFreqs);
    QVector<double> trimVector(const QVector<double>& data,
               const QVector<int>& ind);
};
#endif // ALGORITHM_THREAD_H
