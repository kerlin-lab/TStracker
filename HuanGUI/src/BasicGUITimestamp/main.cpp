// Standard Lib
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

// Spinview lib
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

// OpenCv lib
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

#define TEXT_OFFSET 10

// Return time from running acquisition in sub milisecond
uint64_t getReadableTimestamp(uint64_t timestamp)
{
    static uint64_t ts=0;
    if(ts==0)
    {
        ts=timestamp;
        return 0;
    }
    else
    {
        return (timestamp-ts)/1000;
    }
}


void drawTime(Mat& frame,double time)
{
    putText(frame,to_string(time)+"s",Point(TEXT_OFFSET,frame.rows - TEXT_OFFSET),FONT_HERSHEY_SIMPLEX,0.5,Scalar(255,255,255));
}

void dbhere(int a=0)
{
    cout<<"Here "<<a<<endl;
}
/*
 *  Return a CONTINUOUS mono 8 Mat
 *  with each pixel of type uchar and 
 *  each size width*height
 */
void createMono8Mat(Mat& frame,int width,int height)
{
    frame.create(height,width,CV_8UC1);
}

/*
 * The function convert a mono 8-bit gray scale image 
 * from Spinview ImagePtr to OpenCV Mat
 * for the sake of performance
 *
 * NO type checking
 * NO size checking 
 * the Input cv_con must be a continuous Mat (use the function createMono8Mat to get one)
 * the size is width * height 
 *
 * User need to do the task before calling the function
 * ideally, the check should be done before acquisition takes place
 */
void ImagePtr2CVMat_CV_8UC1(ImagePtr& spin_con,Mat& cv_con,int size)
{
    uchar* rawData=(uchar*)spin_con->GetData();
    uchar* p=cv_con.ptr<uchar>(0);
    while(size--)
    {
        *(p++)=*(rawData++);
    }
}

// This function acquires and saves 10 images from a device.
int AcquireAndShowImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice)
{
    int result = 0;
    int imgWidth,imgHeight,imgSize;
    Mat frame;
    uint64_t timestamp;

    cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;

    try
    {

        // Retrieve enumeration node from nodemap
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
        {
            cout << "Unable to set acquisition mode to continuous (enum retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Retrieve entry node from enumeration node
        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
        {
            cout << "Unable to set acquisition mode to continuous (entry retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Retrieve integer value from entry node
        const int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

        // Set integer value from entry node as new value of enumeration node
        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

        cout << "Acquisition mode set to continuous..." << endl;

        // Turn off trigger, otherwise ,
        // "Failed waiting for EventData on NEW_BUFFER_DATA event" error will happen
        cout<<"Turn off trigger mode"<<endl;
        pCam->TriggerMode.SetValue(Spinnaker::TriggerModeEnums::TriggerMode_Off);

        // Get image width and height in the setting
        CIntegerPtr width = nodeMap.GetNode("Width");
        CIntegerPtr height = nodeMap.GetNode("Height");
        
        imgWidth=width->GetValue();
        imgHeight=height->GetValue();
        imgSize=imgWidth*imgHeight;

        cout<<"Image width value from Camera Setting: "<<imgWidth<<endl;
        cout<<"Image height value from Camera Setting: "<<imgHeight<<endl;

        // Reserve memmory that OpenCv will use to hold the image
        createMono8Mat(frame,imgWidth,imgHeight);


        //
        // Begin acquiring images
        //
        // *** NOTES ***
        // What happens when the camera begins acquiring images depends on the
        // acquisition mode. Single frame captures only a single image, multi
        // frame captures a set number of images, and continuous captures a
        // continuous stream of images. Because the example calls for the
        // retrieval of 10 images, continuous mode has been set.
        //
        // *** LATER ***
        // Image acquisition must be ended when no more images are needed.
        //

        pCam->BeginAcquisition();

        cout << "Acquiring images..." << endl;


        //
        // Retrieve device serial number for filename
        //
        // *** NOTES ***
        // The device serial number is retrieved in order to keep cameras from
        // overwriting one another. Grabbing image IDs could also accomplish
        // this.
        //
        gcstring deviceSerialNumber("");
        CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
        if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
        {
            deviceSerialNumber = ptrStringSerial->GetValue();

            cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
        }
        cout << endl;


        while(true)
        {
            try
            {
                //
                // Retrieve next received image
                //
                // *** NOTES ***
                // Capturing an image houses images on the camera buffer. Trying
                // to capture an image that does not exist will hang the camera.
                //
                // *** LATER ***
                // Once an image from the buffer is saved and/or no longer
                // needed, the image must be released in order to keep the
                // buffer from filling up.
                //
                ImagePtr pResultImage = pCam->GetNextImage(1000);

                //
                // Ensure image completion
                //
                // *** NOTES ***
                // Images can easily be checked for completion. This should be
                // done whenever a complete image is expected or required.
                // Further, check image status for a little more insight into
                // why an image is incomplete.
                //
                if (pResultImage->IsIncomplete())
                {
                    // Retrieve and print the image status description
                    cout << "Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
                         << "..." << endl
                         << endl;
                }
                else
                {

                    ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);

                    // Converting to OpenCV Mat
                    ImagePtr2CVMat_CV_8UC1(convertedImage,frame,imgSize);
                    // Get timestamp
                    timestamp=convertedImage->GetTimeStamp();
                    // Draw timestamp
                    drawTime(frame,getReadableTimestamp(timestamp)/1000000.0);
                    imshow("Picture", frame);
                    if(waitKey(1)>=0)
                    {
                        break;
                    }
                }

                //
                // Release image
                //
                // *** NOTES ***
                // Images retrieved directly from the camera (i.e. non-converted
                // images) need to be released in order to keep from filling the
                // buffer.
                //
                pResultImage->Release();
            }
            catch (Spinnaker::Exception& e)
            {
                cout << "Error: " << e.what() << endl;
                result = -1;
            }
        }

        // Release matrix to save memmory
        frame.release();
        //
        // End acquisition
        //
        // *** NOTES ***
        // Ending acquisition appropriately helps ensure that devices clean up
        // properly and do not need to be power-cycled to maintain integrity.
        //

        pCam->EndAcquisition();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        return -1;
    }


    return result;
}

// This function acts as the body of the example; please see NodeMapInfo example
// for more in-depth comments on setting up cameras.
int RunAcquisition(CameraPtr pCam)
{
    int result;

    try
    {
        // Retrieve TL device nodemap and print device information
        INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();

        // Initialize camera
        pCam->Init();

        // Retrieve GenICam nodemap
        INodeMap& nodeMap = pCam->GetNodeMap();

        // Acquire images
        result = result | AcquireAndShowImages(pCam, nodeMap, nodeMapTLDevice);

        // Deinitialize camera
        pCam->DeInit();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// Example entry point; please see Enumeration example for more in-depth
// comments on preparing and cleaning up the system.
int main(int /*argc*/, char** /*argv*/)
{
    int result;
    // Retrieve singleton reference to system object
    SystemPtr system = System::GetInstance();

    // Retrieve list of cameras from the system
    CameraList camList = system->GetCameras();

    const unsigned int numCameras = camList.GetSize();

    cout << "Number of cameras detected: " << numCameras << endl << endl;

    // Finish if there are no cameras
    if (numCameras == 0)
    {
        // Clear camera list before releasing system
        camList.Clear();

        // Release system
        system->ReleaseInstance();

        cout << "Not enough cameras!" << endl;
        cout << "Done! Press Enter to exit..." << endl;
        getchar();

        return -1;
    }
    else
    {
        // Camera found, so run

        CameraPtr pCam = nullptr;


        // use the first camera found
        pCam = camList.GetByIndex(0);

        // Run acquisition
        RunAcquisition(pCam);

        // Clean UP
        pCam = nullptr;

        // Clear camera list before releasing system
        camList.Clear();

        // Release system
        system->ReleaseInstance();

        cout << endl << "Done! Press Enter to exit..." << endl;
        getchar();

    }

    return result;
}