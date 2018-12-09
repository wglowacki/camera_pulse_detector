#include "functions_and_tools.h"

void tools::displayCustomMessage(std::string box_title, std::string content)
{
    QMessageBox messageBox;
    messageBox.setInformativeText(QString::fromStdString(content));
    messageBox.setText(QString::fromStdString(box_title));
    messageBox.exec();
}
double tools::timespaceInSeconds(QTime& end_time, QTime& start_time)
{
    int miliseconds_elapsed = end_time.msecsTo(start_time);
    return (static_cast<double>(miliseconds_elapsed)/1000.0);
}
