#pragma once

#include <pylon/PylonIncludes.h>
#include <opencv2/opencv.hpp>

struct PylonCamProperties {
    size_t numImgsToCatch = std::numeric_limits<int>::max();
};

class CameraPylon
{
public:
    CameraPylon();
    ~CameraPylon();

    void setConfigData();
    void openCam();
    bool isOpened();
    void disconnectDevice();
    void getCvFrame(cv::Mat& singleFrame, uint64_t& exposureTime);

private:
    std::shared_ptr<Pylon::CInstantCamera> camera;
    PylonCamProperties camProp;
    Pylon::CGrabResultPtr ptrGrabResult;

    Pylon::CImageFormatConverter opencvConverter;
    Pylon::CPylonImage cvImgHandler;
};
