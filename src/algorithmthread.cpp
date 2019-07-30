#include "algorithmthread.h"

AlgorithmThread::AlgorithmThread(QObject * parent)
    : QThread(parent)
{
}

void AlgorithmThread::run()
{
    if(detectedFaces == nullptr) {
        return;
    }
}

