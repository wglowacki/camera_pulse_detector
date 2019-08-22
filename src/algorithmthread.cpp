#include "algorithmthread.h"

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
    while(true){
        if(foreheadBuff.empty()) {
            return;
        }
        //!!!!!!!!!!!!!!!!!!!!!!!!
        //Everyting as for now is done for one face detected
        auto meansVect = calcRoiMeans();
        int sampLen = meansVect.size();
        if(sampLen < minSize) {
            msleep(100);
            qDebug() << "Wait for buffer";
        }
        else {
            /*HERE FPS COUNT*/
            //calc linspace timeshifts
            auto vectEvenTimes = calcLinspaceTimes(sampLen);
            //Interpolate linspace mean values
            auto vectInterpMeans = calcInterpMeans(vectEvenTimes, meansVect);
            auto hw = tools::createHammingWindow(sampLen);
            auto vectHamMeans = tools::multiplyVec(vectInterpMeans, hw);
            auto normalizedMeans = tools::normalize(vectHamMeans);
            // Get absolute values of FFT coefficients
            auto vectfft = calcFFT(normalizedMeans);
            auto vectfftAngles = calcComplexFftAngles(vectfft);
            auto vectfftAbs = calcComplexFftAbs(vectfft);
            qDebug() << normalizedMeans.last();
        }
        qDebug() << meansVect.back();
        qDebug() << "here";
        msleep(100);
    }
}

void AlgorithmThread::setForeheadBuffer(
        QVector<std::shared_ptr<FrameBuffer>> &frameBuffer)
{
    foreheadBuff = frameBuffer;
}

QVector<double> AlgorithmThread::calcRoiMeans()
{
    /*CALCULATED ONLY FOR FIRST FRAME*/
    QVector<double> meansVect;
    std::for_each(foreheadBuff.begin(), foreheadBuff.end(),
              [&](std::shared_ptr<FrameBuffer> fh)
    {
        std::for_each(fh->buffer.begin(), fh->buffer.end(),
            [&](std::pair<cv::Mat, int> frame)
        {
            imshow("name", frame.first);
            auto cvMat = frame.first;
            auto scalarMeans = cv::mean(cvMat);
            meansVect.push_back(scalarMeans.val[0]);
//            qDebug() << meansVect.back() << meansVect.size();
//            qDebug() << scalarMeans.val[0] << scalarMeans.val[2];
        }
        );
    });
    return meansVect;
}

QVector<double> AlgorithmThread::calcLinspaceTimes(int vectSize)
{
    QVector<double> evenTimes(vectSize);
    double startT = foreheadBuff.first()->buffer.first().second;
    double endT = foreheadBuff.last()->buffer.last().second;
    double timeGap = (endT - startT) / (vectSize - 1);
    assert(timeGap);
    evenTimes.first() = startT;
    for (int i=1; i < (vectSize-1); ++i)
    {
        evenTimes[i] = evenTimes.at(i-1) + timeGap;
    }
    evenTimes.last() = endT;
    return evenTimes;
}

//
// Transform data to FFT
//http://www.gnu.org/software/gsl/manual/gsl-ref_16.html
//
QVector<gsl_complex> AlgorithmThread::calcFFT(const QVector<double>& vectMeans)
{
    int size = vectMeans.size();
    double data[size];
    std::copy(vectMeans.begin(), vectMeans.end(), data);
    // Transform to fft
    gsl_fft_real_workspace* work = gsl_fft_real_workspace_alloc(size);
    gsl_fft_real_wavetable* real = gsl_fft_real_wavetable_alloc(size);
    gsl_fft_real_transform(data, 1, size, real, work);
    gsl_fft_real_wavetable_free(real);
    gsl_fft_real_workspace_free(work);
    // Unpack complex numbers
    gsl_complex unpacked[size];
    gsl_fft_halfcomplex_unpack(data, (double *) unpacked, 1, size);
    // Copy to  a vector
    int unpacked_size = size / 2 + 1;
    std::vector<gsl_complex> stdVect(unpacked, unpacked + unpacked_size);
    QVector<gsl_complex> output;
    output.fromStdVector(stdVect);
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

    // BUGFIX: Need to iterate throuh given x-interpolation range
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
