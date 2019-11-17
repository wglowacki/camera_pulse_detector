#include <assert.h>
#include "functionsandtools.h"
#include <QtMath>
#include <QDebug>

void tools::dispQMsg(QString title, QString content)
{
    QMessageBox messageBox;
    messageBox.setInformativeText(content);
    messageBox.setText(title);
    messageBox.exec();
}

std::vector<double> tools::createHammingWindow(int len)
{
    std::vector<double> hw(len);
    if (len == 1) {
        hw.front() = 1.0;
    }
    else {
        for (int i = 0; i < len; ++i) {
            hw.at(i) = 0.54 - 0.46*cos( (2*M_PI*i)/(len-1) );
        }
    }
    return hw;
}

std::vector<double> tools::normalize(const std::vector<double>& vect)
{
    double sum = std::accumulate(vect.begin(), vect.end(), 0.0);
    double mean = sum / vect.size();

    std::vector<double> diff(vect.size());
    std::transform(vect.begin(), vect.end(), diff.begin(),
                   std::bind2nd(std::minus<double>(), mean));
    double sqSum = std::inner_product(diff.begin(), diff.end(),
                                      diff.begin(), 0.0);
    double stdev = std::sqrt(sqSum / vect.size());

    std::vector<double> retVect;
//    double mean = std::accumulate(vect.begin(),vect.end(),0.0)/vect.size();
    for (auto& i : vect) {
        retVect.push_back((i-mean)/stdev);
    }
    return retVect;
}


QVector<double> tools::multiplyVec(const QVector<double>& first,
                            const QVector<double>& second)
{
    assert (first.size() == second.size());
    QVector<double> val;
    for (int i = 0; i < first.size(); ++i)
    {
        val.push_back(first.at(i) * second.at(i));
    }
    return val;
}

double tools::timeInSec(QTime& endTime, QTime& startTime)
{
    int milisecondsElapsed = endTime.msecsTo(startTime);
    return (static_cast<double>(milisecondsElapsed)/1000.0);
}
