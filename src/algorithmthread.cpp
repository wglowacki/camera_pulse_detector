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
    QMutex mtx;
    while(true)
    {

        if(foreheadBuff.empty()) {
            qDebug() << "waiting for detected frames.";
            msleep(300);
        }
        if(!flagReceivedNewImage) {
            msleep(40);
        }
        flagReceivedNewImage = false;
        currentFHBuff = foreheadBuff;
        auto meansVect = calcRoiMeans();
        int sampLen = meansVect.size();
        detrendSignal(meansVect, sampLen);

        if(sampLen < minSize) {
            msleep(100);
            qDebug() << "Wait for buffer";
            continue;
        }
        double startT = currentFHBuff.first()->buffer.first().second;
        double endT = currentFHBuff.last()->buffer.last().second;
        double timeDif = endT - startT;
        if(timeDif <= 0.0) {
            qDebug() << "Time difference: " << timeDif;
            continue;
        }
        double freq = sampLen / timeDif;

        auto vectEvenTimes =
                matrix_operations::calcLinspaceTimes(
                    sampLen, startT, endT
                );
        auto vectInterpMeans = calcInterpMeans(
                    vectEvenTimes, meansVect
        );
        auto hw = tools::createHammingWindow(sampLen);
        auto vectHamMeans =
                matrix_operations::multiplyVecKernel (
                    vectInterpMeans, hw
                );
        auto normalizedMeans = tools::normalize(vectHamMeans);
        // Get absolute values of FFT coefficients
//        auto vectfft = calcFFT(normalizedMeans);
//        auto vectfftAbs = calcComplexFftAbs(vectfft);
        std::vector<double> vectfftAbs;

        matrix_operations::gpuCalcFFT(normalizedMeans, vectfftAbs);
        if(!vectfftAbs.size()) {
//            std::cout << "error catched";
            continue;
        }
        // Get indices of frequences that are less than 50 and greater than 150
        auto filteredFreqs = getDesiredFreqs(sampLen, freq);
        auto filteredIndexes = trimFreqs(filteredFreqs);

        auto fftabs = trimVector(vectfftAbs, filteredIndexes);
        auto freqs  = trimVector(filteredFreqs, filteredIndexes);

        //CPU
        /*
        unsigned maxFftAbsIndex =
                static_cast<unsigned> (std::distance(fftabs.begin(),
                std::max_element(fftabs.begin(), fftabs.end()) ) );
        */
        //GPU
        unsigned maxFftAbsIndex = matrix_operations::maxIndex(fftabs);
        vectBPM.push_back(freqs.at(maxFftAbsIndex));

        if(saveFlag) {
            mtx.lock();
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%M-%S");
            std::string timeStamp = oss.str();
            saveFile << timeStamp << " " << vectBPM.back() << "\n";
            mtx.unlock();
        }

        static int cnt = 0;
        if(cnt > 5) {
            updateBpm(vectBPM);
            cnt = -1;
        }
        ++cnt;

//      debugLog();
    }
}

std::vector<double>
AlgorithmThread::trimVector ( const std::vector<double>& data,
                              const std::vector<int>& ind )
{
    std::vector<double> trimmed;
    for (auto const &i : ind)
    {
        trimmed.push_back(data.at(i));
    }
    return trimmed;
}

void AlgorithmThread::detrendSignal(std::vector<double>& means, int size)
{
    std::vector<double> detrendedMeans;
    int kernel = 15;
    for (int i=0; i< size; ++i)
    {
        if(i<5) {
            detrendedMeans.push_back(
                means.at(i) - 1/(2*kernel+1) *
                        std::accumulate( means.begin()+i,
                                         means.begin()+(i+2*kernel), 0.0)
            );
        } else if (i < size-5) {
            detrendedMeans.push_back(
                means.at(i) - 1/(2*kernel+1) *
                        std::accumulate( means.begin()+(i-kernel),
                                         means.begin()+(i+kernel), 0.0)
            );
        } else {
            detrendedMeans.push_back(
                means.at(i) - 1/(2*kernel+1) *
                    std::accumulate( means.end()-(i+2*kernel),
                                     means.end()-i, 0.0)
            );
        }

    }
    means = detrendedMeans;
}

void AlgorithmThread::setForeheadBuffer(
        QVector<std::shared_ptr<FrameBuffer>> &frameBuffer)
{
    foreheadBuff = frameBuffer;
}

void AlgorithmThread::setImageReceivedFlag(bool& sharedFlag)
{
    flagReceivedNewImage = sharedFlag;
}

std::vector<double> AlgorithmThread::calcRoiMeans()
{
    threadMutex.lock();
    std::vector<double> meansVect;
    int i = 0;
    std::for_each(
        currentFHBuff.front()->buffer.begin(),
        currentFHBuff.front()->buffer.end(),
        [&](std::pair<cv::Mat, int> frame)
    {
        cv::Mat cvMat = frame.first;
        auto scalarMeans = cv::mean(cvMat);
        meansVect.push_back(scalarMeans.val[i]);
    }
    );
    threadMutex.unlock();
    return meansVect;
}

QVector<double>
AlgorithmThread::calcLinspaceTimes(
        int vectSize, double startT, double endT)
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

std::vector<gsl_complex>
AlgorithmThread::calcFFT(std::vector<double>& vectMeans)
{
    size_t size = vectMeans.size();
    double data[size];
    std::copy(vectMeans.begin(), vectMeans.end(), data);

    gsl_fft_real_wavetable* real = gsl_fft_real_wavetable_alloc(size);
    gsl_fft_real_workspace* work = gsl_fft_real_workspace_alloc(size);
    gsl_fft_real_transform(data, 1, size, real, work);
    gsl_fft_real_wavetable_free(real);
    gsl_fft_real_workspace_free(work);
    gsl_complex unpacked[size];
    gsl_fft_halfcomplex_unpack(data, (double*) unpacked, 1, size);
    int newSize = size / 2 + 1;
    std::vector<gsl_complex> stdVect(unpacked, unpacked + newSize);

    return stdVect;
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

std::vector<double> AlgorithmThread::calcComplexFftAbs(
        const std::vector<gsl_complex>& fftraw)
{
    std::vector<double> output;
    for (auto&& fft : fftraw) {
        output.push_back(gsl_complex_abs(fft));
    }
    return output;
}

std::vector<double>
AlgorithmThread::getDesiredFreqs(int size, double freq)
{
    // Frequencies using spaced values within interval - L/2+1
    int newSize = (size / 2) + 1;
    std::vector<double> genFreqs(size);
    double freqGen = freq/size;
    int cnt=0;
    for(auto& freq : genFreqs)
    {
        freq = freqGen* (++cnt)*60;
    }
    return genFreqs;
}

std::vector<int>
AlgorithmThread::trimFreqs(const std::vector<double>& genFreqs)
{
    std::vector<int> filteredFreqs;
    for(int i = 0; i < genFreqs.size(); ++i)
    {
        if(genFreqs.at(i) > BPM_FILTER_LOW &&
            genFreqs.at(i) < BPM_FILTER_HIGH)
        {
            filteredFreqs.push_back(i);
        }
    }
    return filteredFreqs;
}

std::vector<double>
AlgorithmThread::calcInterpMeans(const std::vector<double>& evenTimes,
                                 std::vector<double>& means )
{
    //https://www.gnu.org/software/gsl/doc/html/interp.html
    assert (evenTimes.size() == means.size());

    std::vector<double> interpRes;

    //    http://www.cplusplus.com/forum/general/216928/
    double data_y_array[means.size()];
    double evenTimes_array[evenTimes.size()];
    std::copy (means.begin(), means.end(), data_y_array);
    std::copy (evenTimes.begin(), evenTimes.end(), evenTimes_array);

    size_t len_even_times = static_cast<size_t>(evenTimes.size());

    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    gsl_spline *spline = gsl_spline_alloc (
                gsl_interp_linear, len_even_times
                );
    gsl_spline_init (spline, evenTimes_array,
                     data_y_array, len_even_times );

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

void AlgorithmThread::startSaveStatus(bool sf, std::string fn)
{
    QMutex mtx;
    mtx.lock();
    saveFlag = sf;
    if(saveFlag) {
        if(saveFile.is_open()) {
            saveFile.close();
        }
        std::string name = "algorithm_" + fn + ".txt";
        saveFile.open(name);
        saveFlag = true;
    }
    else {
        saveFlag = false;
        saveFile.close();
    }
    mtx.unlock();
}

void AlgorithmThread::updateBpm(const std::vector<double>& v)
{
    double ret = 0;
    if(v.size() > 20) {
        ret = std::accumulate( v.end()-20, v.end(), 0.0) / 20;
    }
    bpsUpdate(ret);
}

void AlgorithmThread::debugLog()
{
    qDebug() << "*****************************************";
    qDebug() << "*****************************************";
    qDebug() << "BPM: " << vectBPM.back();

    qDebug() << "*****************************************";
    qDebug() << "*****************************************";
}

void AlgorithmThread::end()
{
    saveFlag = false;
    saveFile.close();
    bpmFile.close();

}
