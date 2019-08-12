#ifndef FUNCTIONS_AND_TOOLS_H
#define FUNCTIONS_AND_TOOLS_H

#include <QMessageBox>
#include <QTime>

namespace tools
{
    void dispQMsg(QString title, QString content);
    double timeInSec(QTime& endTime, QTime& startTime);
}

#endif // FUNCTIONS_AND_TOOLS_H
