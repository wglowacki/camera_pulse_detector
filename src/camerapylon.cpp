#include "camerapylon.h"
#include "limits"

#include <iomanip>

CameraPylon::CameraPylon()
{
    Pylon::PylonInitialize();
    camera = std::make_shared<Pylon::CInstantCamera>
                    ( Pylon::CTlFactory::GetInstance().CreateFirstDevice() );
    // Print the model name of the camera.
    std::cout << "Using device " << camera->GetDeviceInfo().GetModelName() << std::endl;
}

void CameraPylon::setConfigData()
{
    //here camera config is initialized
    std::cout << "\nSet camera configuration data\n";

    const Pylon::String_t& fn = "/home/agh/acA2040-90uc_22924816.pfs";
    try {
        Pylon::CFeaturePersistence::Load(fn, &camera->GetNodeMap(), true);
    } catch (const Pylon::GenericException& exception) {
        std::cerr << exception.GetDescription();
    }
}

void CameraPylon::openCam()
{
    camera->Open();
    setConfigData();
    camera->StartGrabbing( camProp.numImgsToCatch, Pylon::GrabStrategy_LatestImages);

    opencvConverter.OutputPixelFormat = Pylon::PixelType_BGR8packed;
}

bool CameraPylon::isOpened()
{
    return camera->IsOpen();
}


void CameraPylon::getCvFrame(cv::Mat& singleFrame, uint64_t& exposureTime)
{
    if(!camera->IsGrabbing()) {
        cv::Mat pylonCvImg;
        singleFrame = pylonCvImg;
        return;
    }
    camera->RetrieveResult( 5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
    // Access the image data.
    const uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
    static const int cameraTic = 1; //duration of one camera tick in ns.
    exposureTime = ptrGrabResult->GetTimeStamp() * cameraTic;
    //CV
    opencvConverter.Convert(cvImgHandler, ptrGrabResult);
    singleFrame = cv::Mat( ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(),
                          CV_8UC3, (uint8_t*)cvImgHandler.GetBuffer() );
    return;
}

void CameraPylon::disconnectDevice()
{
    if(camera->IsOpen()) {
        camera->Close();
    }
    Pylon::PylonTerminate();
}

CameraPylon::~CameraPylon()
{
    disconnectDevice();
}
