#ifndef FUNCTIONS_AND_TOOLS_H
#define FUNCTIONS_AND_TOOLS_H

#include <QMessageBox>
#include <QTime>
#include <iostream>

namespace tools {
    void displayCustomMessage(std::string box_title, std::string content);
    double timespaceInSeconds(QTime& end_time, QTime& start_time);
}

#endif // FUNCTIONS_AND_TOOLS_H
