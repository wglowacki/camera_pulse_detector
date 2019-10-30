QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CameraPulseDetector
TEMPLATE = app
DESTDIR = ./release
CONFIG += c++14 release
RESOURCES += ./qdarkstyle/style.qrc
OBJECTS_DIR += build
UI_DIR += ./ui

INCLUDEPATH += . \
    /usr/local/include \
    ./include 
    ./src

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/algorithmthread.cpp \
    src/functionsandtools.cpp \
    src/camerathread.cpp \
    src/framebuffer.cpp \
    src/facedetectorthread.cpp

CUDA_SOURCES += src/matrixoperations.cu
HEADERS += \
    ./include/mainwindow.h \
    ./include/algorithmthread.h \
    ./include/functionsandtools.h \
    ./include/camerathread.h \
    ./include/framebuffer.h \
    ./include/facedetectorthread.h \
    ./include/matrixoperations.h

FORMS += \
     ./ui/mainwindow.ui

LIBS += \
    -L/usr/local/lib -L/usr/local/cuda/lib64 \
    -lcudart \
    -lopencv_core -lopencv_highgui -lopencv_calib3d \
    -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs \
    -lopencv_cudaimgproc -lopencv_cudaobjdetect \
    -lgsl -lgslcblas
