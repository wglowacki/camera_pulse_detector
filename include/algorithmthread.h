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

    void setImageReceivedFlag(bool& flagReceivedNewImage);
    void startSaveStatus(bool saveFlag, std::string fn);
    void run() override;

private:
    bool saveFlag = false;
    std::ofstream saveFile;
    const int BPM_FILTER_LOW   = 55;
    const int BPM_FILTER_HIGH = 170;
    int minSize = 50;
    QMutex threadMutex;
    QVector<std::shared_ptr<FrameBuffer>> foreheadBuff;
    QVector<std::shared_ptr<FrameBuffer>> currentFHBuff;
    bool flagReceivedNewImage = false;
    std::ofstream bpmFile;
    std::vector<double> vectBPM;

    void detrendSignal(std::vector<double>& means, int size);
    std::vector<double> calcRoiMeans();
    QVector<double> calcLinspaceTimes (
            int vectSize, double startT, double endT
    );

    std::vector<double> calcInterpMeans (
            const std::vector<double> & evenTimes,
            std::vector<double>& means
    );

    std::vector<gsl_complex>
    calcFFT(std::vector<double> &vectMeans);

    QVector<double> calcComplexFftAngles(
            const QVector<gsl_complex>& fftraw
    );

    std::vector<double> calcComplexFftAbs(
            const std::vector<gsl_complex>& fftraw
    );

    std::vector<double>
    getDesiredFreqs(int size, double freq);

    std::vector<int>
    trimFreqs(const std::vector<double>& genFreqs);

    std::vector<double>
    trimVector(const std::vector<double>& data,
               const std::vector<int>& ind);

    void updateBpm(const std::vector<double>& vectBPM);
    void debugLog();
};
#endif // ALGORITHM_THREAD_H
