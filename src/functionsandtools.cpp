#include "functionsandtools.h"

void tools::dispQMsg(QString title, QString content)
{
    QMessageBox messageBox;
    messageBox.setInformativeText(content);
    messageBox.setText(title);
    messageBox.exec();
}
double tools::timeInSec(QTime& endTime, QTime& startTime)
{
    int milisecondsElapsed = endTime.msecsTo(startTime);
    return (static_cast<double>(milisecondsElapsed)/1000.0);
}
