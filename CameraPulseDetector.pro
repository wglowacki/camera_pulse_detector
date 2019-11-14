QT       += core gui serialport

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
    /usr/local/opencv/include \
    /opt/pylon5/include \
    ./include \

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/algorithmthread.cpp \
    src/functionsandtools.cpp \
    src/camerathread.cpp \
    src/camerapylon.cpp \
    src/framebuffer.cpp \
    src/facedetectorthread.cpp \
    src/referencesensor.cpp

CUDA_OBJECTS_DIR = OBJECTS_DIR/cuda
CUDA_SOURCES = src/matrixoperations.cu
CUDA_DIR = /usr/local/cuda
INCLUDEPATH  += $$CUDA_DIR/include
QMAKE_LIBDIR += $$CUDA_DIR/lib64
LIBS += -lcudart -lcuda -lcufft
# GPU architecture
CUDA_ARCH = sm_30
NVCCFLAGS = --compiler-options -fno-strict-aliasing -use_fast_math --ptxas-options=-v
# Prepare the extra compiler configuration (taken from the nvidia forum - i'm not an expert in this part)
CUDA_INC = $$join(INCLUDEPATH,' -I','-I',' ')

cuda.commands = $$CUDA_DIR/bin/nvcc -m64 -O3 -arch=$$CUDA_ARCH -c $$NVCCFLAGS \
                $$CUDA_INC $$LIBS  ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT} \
                2>&1 | sed -r \"s/\\(([0-9]+)\\)/:\\1/g\" 1>&2
# nvcc error printout format ever so slightly different from gcc
# http://forums.nvidia.com/index.php?showtopic=171651

cuda.dependency_type = TYPE_C # there was a typo here. Thanks workmate!
cuda.depend_command = $$CUDA_DIR/bin/nvcc -O3 -M $$CUDA_INC $$NVCCFLAGS   ${QMAKE_FILE_NAME}

cuda.input = CUDA_SOURCES
cuda.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}_cuda.o
# Tell Qt that we want add more stuff to the Makefile
QMAKE_EXTRA_COMPILERS += cuda

HEADERS += \
    ./include/mainwindow.h \
    ./include/algorithmthread.h \
    ./include/functionsandtools.h \
    ./include/camerathread.h \
    ./include/camerapylon.h \
    ./include/framebuffer.h \
    ./include/facedetectorthread.h \
    ./include/matrixoperations.h \
    ./include/referencesensor.h

FORMS += \
     ./ui/mainwindow.ui


LIBS += -L$$CUDA_DIR/lib64 -lcuda -lcudart
LIBS += \
    -L"/opt/pylon5/lib64" \
    -L/usr/local/lib \
    -L/usr/local/opencv/lib \
    -lopencv_core -lopencv_highgui -lopencv_calib3d \
    -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs \
    -lopencv_cudaimgproc -lopencv_cudaobjdetect \
    -lgsl -lgslcblas \
    -lpylonutility -lpylonbase -lGCBase_gcc_v3_1_Basler_pylon
