#include "algorithmthread.h"

#include <limits>
#include <algorithm>
#include <iterator>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

AlgorithmThread::AlgorithmThread(QObject * parent)
    : QThread(parent)
{
}

void AlgorithmThread::run()
{
    while(true)
    {
        if(foreheadBuff.empty()) {
            qDebug() << "waiting for detected frames.";
            msleep(300);
        }
        currentFHBuff = foreheadBuff;
        auto meansVect = calcRoiMeans();
        int sampLen = meansVect.size();

        if(sampLen < minSize) {
            msleep(100);
            qDebug() << "Wait for buffer";
        } else {
            double timeDif = currentFHBuff.first()->buffer.back().second -
                             currentFHBuff.first()->buffer.first().second;
            if(timeDif <= 0.0) {
                qDebug() << "Time difference: " << timeDif;
                continue;
            }
            double freq = sampLen / timeDif;
            double startT = currentFHBuff.first()->buffer.first().second;
            double endT = currentFHBuff.last()->buffer.last().second;

            auto stdVectEvenTimes = matrix_operations::calcLinspaceTimes(sampLen, startT, endT);

            auto vectEvenTimes = QVector<double>::fromStdVector(stdVectEvenTimes);
            //Interpolate linspace mean values
            auto vectInterpMeans = calcInterpMeans(vectEvenTimes, meansVect);
            auto hw = tools::createHammingWindow(sampLen).toStdVector();
            auto stdVectHamMeans = matrix_operations::multiplyVecKernel(vectInterpMeans.toStdVector(), hw);
            auto vectHamMeans = QVector<double>::fromStdVector(stdVectHamMeans);
            auto normalizedMeans = tools::normalize(vectHamMeans);
//            auto normalizedMeans = QVector<double>::fromStdVector(normalizedMeans);
            // Get absolute values of FFT coefficients
            auto vectfft = calcFFT(normalizedMeans);
            auto vectfftAbs = calcComplexFftAbs(vectfft);
            // Get indices of frequences that are less than 50 and greater than 150
            auto filteredFreqs = getDesiredFreqs(sampLen, freq);
            auto filteredIndexes = trimFreqs(filteredFreqs);

            auto fftabs = trimVector(vectfftAbs, filteredIndexes);
            auto freqs  = trimVector(filteredFreqs, filteredIndexes);

            int maxFftAbsIndex = static_cast<int> (std::distance(fftabs.begin(),
                    std::max_element(fftabs.begin(), fftabs.end()) ) );

            vectBPM.push_back(freqs.at(maxFftAbsIndex));
            bpsUpdate(vectBPM.back());
            qDebug() << "*****************************************";
            qDebug() << "*****************************************";
            qDebug() << "BPM: " << vectBPM.back();
            qDebug() << "Max FFT ABS: " << fftabs.at(maxFftAbsIndex);

            qDebug() << "*****************************************";
            qDebug() << "*****************************************";
        }
    }
}

QVector<double> AlgorithmThread::trimVector (
        const QVector<double>& data, const QVector<int>& ind )
{
    QVector<double> trimmed;
    for (auto const &i : ind)
    {
        trimmed.push_back(data.at(i));
    }
    return trimmed;
}

void AlgorithmThread::setForeheadBuffer(
        QVector<std::shared_ptr<FrameBuffer>> &frameBuffer)
{
    foreheadBuff = frameBuffer;
}

QVector<double> AlgorithmThread::calcRoiMeans()
{
    threadMutex.lock();
    QVector<QVector<double>> meansVect;
    QVector<double> singleVect;
    int i = 0;
    std::for_each(currentFHBuff.begin(), currentFHBuff.end(),
              [&](std::shared_ptr<FrameBuffer> fh)
    {
        std::for_each(fh->buffer.begin(), fh->buffer.end(),
            [&](std::pair<cv::Mat, int> frame)
        {
            cv::Mat cvMat = frame.first;
            auto scalarMeans = cv::mean(cvMat);
            singleVect.push_back(scalarMeans.val[i]);
//            qDebug() << meansVect.back() << meansVect.size();
//            qDebug() << scalarMeans.val[0] << scalarMeans.val[2];
        }
        );
        meansVect.push_back(singleVect);
        ++i;
        singleVect.clear();
    });
    threadMutex.unlock();
    return meansVect.first();
}

QVector<double> AlgorithmThread::calcLinspaceTimes(int vectSize, double startT, double endT)
{
    QVector<double> evenTimes(vectSize);
    double timeGap = (endT - startT) / (vectSize - 1);
    assert(timeGap>0);
    evenTimes.first() = startT;
    for (int i=1; i < (vectSize-1); ++i)
    {
        evenTimes[i] = evenTimes.at(i-1) + timeGap;
    }
    evenTimes.last() = endT;
    return evenTimes;
}

QVector<gsl_complex> AlgorithmThread::calcFFT(const QVector<double>& vectMeans)
{
    size_t size = vectMeans.size();
//    qDebug() << "size";
//    qDebug() << size;
    double data[size];
    std::copy(vectMeans.begin(), vectMeans.end(), data);
//    qDebug() << data[1];
//    qDebug() << data[size/2];

    gsl_complex_packed_array gslData = data;
    gsl_fft_real_wavetable* real = gsl_fft_real_wavetable_alloc(size);
    gsl_fft_real_workspace* work = gsl_fft_real_workspace_alloc(size);
    gsl_fft_real_transform(data, 1, size, real, work);
    gsl_fft_real_wavetable_free(real);
    gsl_fft_real_workspace_free(work);
//    qDebug() << data[1];
//    qDebug() << data[size/2];
    // Unpack complex numbers
    gsl_complex unpacked[size];
    gsl_fft_halfcomplex_unpack(data, (double*) unpacked, 1, size);
    // Copy to  a vector
    int unpacked_size = size / 2 + 1;
    std::vector<gsl_complex> stdVect(unpacked, unpacked + size);
    QVector<gsl_complex> output =  QVector<gsl_complex>::fromStdVector(stdVect);

    for (int i=0; i<unpacked_size; i++) {
//        qDebug() << output[i].dat[1];
//        qDebug() << output[i].dat[0];
    }
//    qDebug() << output[size/2].dat[0];
//    qDebug() << "**************";
//    qDebug() << "**************";
//    qDebug() << "**************";
//    qDebug() << "**************";
    return output;
}

QVector<double> AlgorithmThread::calcComplexFftAngles(
        const QVector<gsl_complex>& fftraw)
{
    QVector<double> output;
	for (auto&& fft : fftraw) {
        output.push_back( atan2(GSL_IMAG(fft), GSL_REAL(fft)) );
	}
	return output;
}

QVector<double> AlgorithmThread::calcComplexFftAbs(
        const QVector<gsl_complex>& fftraw)
{
    QVector<double> output;
    for (auto&& fft : fftraw) {
        output.push_back(gsl_complex_abs(fft));
    }
	return output;
}

QVector<double> AlgorithmThread::getDesiredFreqs(int size, double freq)
{
    // Frequencies using spaced values within interval - L/2+1
    int newSize = (size / 2) + 1;
    QVector<double> genFreqs(newSize);
    double freqGen = freq/newSize;
    int cnt=0;
    for(auto& freq : genFreqs)
    {
        freq = freqGen* (++cnt)*60;
    }
    return genFreqs;
}

QVector<int> AlgorithmThread::trimFreqs(const QVector<double>& genFreqs)
{
    // Filter out frequencies less than 50 and greater than 180
    constexpr int BPM_FILTER_LOW = 50;
    constexpr int BPM_FILTER_HIGH = 180;
    QVector<int> filteredFreqs;
    for(int i = 0; i < genFreqs.size(); ++i)
    {
//        qDebug() << genFreqs.at(i);
//        qDebug() << genFreqs.at(i)/60;
        if(genFreqs.at(i) > BPM_FILTER_LOW && genFreqs.at(i) < BPM_FILTER_HIGH) {
            filteredFreqs.push_back(i);
        }
    }
    return filteredFreqs;
}

QVector<double> AlgorithmThread::calcInterpMeans(
        const QVector<double>& evenTimes, QVector<double>& means
)
{
    //https://www.gnu.org/software/gsl/doc/html/interp.html
    assert (evenTimes.size() == means.size());

    QVector<double> interpRes;

    //    http://www.cplusplus.com/forum/general/216928/
    double data_y_array[means.size()];
    double evenTimes_array[evenTimes.size()];
    std::copy (means.begin(), means.end(), data_y_array);
    std::copy (evenTimes.begin(), evenTimes.end(), evenTimes_array);

    size_t len_even_times = static_cast<size_t>(evenTimes.size());

    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    gsl_spline *spline = gsl_spline_alloc (gsl_interp_linear, len_even_times);
    gsl_spline_init (spline, evenTimes_array, data_y_array, len_even_times);

    for(int xi = 0; xi < evenTimes.size(); xi++)
    {
        interpRes.push_back(
                    gsl_spline_eval(spline, evenTimes[xi], acc)
        );
    }

    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);

    return interpRes;
}

void AlgorithmThread::end()
{

}
