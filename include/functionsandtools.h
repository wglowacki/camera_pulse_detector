#ifndef FUNCTIONS_AND_TOOLS_H
#define FUNCTIONS_AND_TOOLS_H

#include <QMessageBox>
#include <QTime>

namespace tools
{
    void dispQMsg(QString title, QString content);
    QVector<double> createHammingWindow(int len);
    QVector<double> normalize(const QVector<double>& vect);
    QVector<double> multiplyVec(const QVector<double>& first,
                                const QVector<double>& second);
    double timeInSec(QTime& endTime, QTime& startTime);
}

#endif // FUNCTIONS_AND_TOOLS_H
