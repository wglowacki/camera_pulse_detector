#include "algorithm_thread.h"

AlgorithmThread::AlgorithmThread(QObject * parent): QThread(parent)
{
}

void AlgorithmThread::run()
{
    if(_detected_faces == nullptr) {
        return;
    }
}

