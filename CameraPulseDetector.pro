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

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    algorithm_thread.cpp \
    functions_and_tools.cpp \
    camera_thread.cpp \
    frame_buffer.cpp \
    face_detector_thread.cpp

HEADERS += \
    ./include/mainwindow.h \
    ./include/algorithm_thread.h \
    ./include/functions_and_tools.h \
    ./include/camera_thread.h \
    ./include/frame_buffer.h \
    ./include/face_detector_thread.h

FORMS += \
     ./ui/mainwindow.ui

LIBS += \
    -L/usr/local/lib -L/usr/local/cuda/lib64 \
    -lcudart \
    -lopencv_core -lopencv_highgui -lopencv_calib3d \
    -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs \
    -lopencv_cudaimgproc -lopencv_cudaobjdetect \
