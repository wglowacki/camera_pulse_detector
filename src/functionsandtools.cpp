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

QVector<double> tools::createHammingWindow(int len)
{
    QVector<double> hw(len);
    if (len == 1) {
        hw.first() = 1.0;
    }
    else {
        for (int i = 0; i < len; ++i) {
            hw[i] = 0.54 - 0.46*cos( (2*M_PI*i)/(len-1) );
        }
    }
    return hw;
}

QVector<double> tools::normalize(const QVector<double>& vect)
{
    QVector<double> retVect;
    double mean = std::accumulate(vect.begin(),vect.end(),0.0)/vect.size();
    for (auto i : vect) {
        retVect.push_back((i-mean));
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
