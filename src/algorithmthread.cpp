#include "algorithmthread.h"

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
        if(meansVect.size() < minSize) {
            msleep(100);
            qDebug() << "Wait for buffer";
        } else {
            //calc linspace timeshifts
            auto vectEvenTimes = calcLinspaceTimes(meansVect.size());
            //Interpolate linspace mean values
            auto vectInterpMeans =
                    calcInterpMeans(vectEvenTimes, meansVect);

        }
        qDebug() << meansVect.back();
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

QVector<double> AlgorithmThread::calcInterpMeans(
        const QVector<double>& evenTimes, QVector<double>& means
)
{
    assert (evenTimes.size() == means.size());
    auto pairFrame = foreheadBuff.first()->buffer;

    QVector<double> interp_y(evenTimes.size());
    QVector<double> interpRes;

    //    http://www.cplusplus.com/forum/general/216928/
    double data_y_array[pairFrame.size()];
    double evenTimes_array[evenTimes.size()];
    std::copy (pairFrame.begin()->second, pairFrame.end()->second, data_y_array);
    std::copy (evenTimes.begin(), evenTimes.end(), evenTimes_array);

    double yi;
    int L = evenTimes.size();

    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    gsl_spline *spline = gsl_spline_alloc (gsl_interp_linear, L);
    gsl_spline_init (spline, evenTimes_array, data_y_array, L);

    // BUGFIX: Need to iterate throuh given x-interpolation range
    for(int xi = 0; xi < evenTimes.size(); xi++)
    {
        yi = gsl_spline_eval (spline, evenTimes[xi], acc);
        interpRes.push_back(yi);
        //printf ("%g\n", yi);
    }

    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);

    return interpRes;
    }
}

void AlgorithmThread::end()
{

}
