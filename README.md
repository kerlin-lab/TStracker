# TStracker
- Version: 2.1.0

## Developing team
- Author: Huan Tran
- Contributor: Harishankar Jayakumar, Tien Dinh

## About
- The goal of TStracker project was to create a recording software that overcomes limits of SpinView software (provided by FLIR along with their Blackfly S camera) and provides better features that more aligned to the tongue-tracking experiment at Kerlin Lab 

## Features
- Simultaneous acquiring from multiple cameras at high speed (up to 1 Khz or may be more if FLIR camera allows)
- Guarantee no delay between recording sessions due to data downloading from cameras (User can start recording a new session right after he/she finishes the last one)
- Auto save images to multi-image TIFF files (tiff stack)
- Retrieve and save images' timestamps in string format and imageID in JSON format to "DateTime" and "ImageDescription" tags of each image in the tiff stacks respectively
- Provide high multi-image TIFF writing speed with auto tiff stack partitioning feature that overcomes the diminishing-write-speed problem of LibTiff when operating on multi-image TIFF files (performance varies from 3Mb/s to 20Mb/s or more depends on the quality of storage device (HDD, SSD, etc))

## Changelog for version 2.1
- Fixed major bug of silent termination while acquiring images after a random number of trials (ThreadSafeQueue::dequeue() had bad design)
- Reassigned the thread priority and made the data flow more logical (ImageMiner > ImageDistributor > the rest)

## Changelog for version 2.0
- Camera acquisition has been moved to dedicated threads to increase image acquiring speed and avoid missing frames.
- Image saving has been moved to dedicated threads providing the ability to write multiple TIFF files concurrently.
- Added auto trial separation feature that detects break interval between trials to start writing to new file

## Project structure
```
Root 
|-- doc 	: this folder contains additional documents to help build the project
|
|-- lib 	: this folder contains libraries used in this project
|
|-- src 	: this is where the source code lies
|   |
|   |-- DevelopmentSuite 	: This contains example source code use to test a specific features
|   |
|   |-- PythonUtilities 	: Utilities that helps with pleminary tasks like concatinating multipile tiff stack to one file, etc
|   |
|   |-- TStracker 		: Go to this directory, read the Readme.md to start building TStracker
```

## Go to the TStracker folder shown in the tree and follow the instructions below to compile the project
### Software needs:
- CMAKE GUI
- Visual Studio (Any version)
- OpenCV 4.3.0 library
- Spinnaker SDK from FLIR camera version 1.29.0.5_x64 (Or any version supporting Visual Studio 2015 toolchain v140)
### Downloading compiling tools
- Get CMAKE GUI at cmake.org
- Get OpenCV at opencv.org/releases/
- Get spinnaker sdk as flir.com/products/spinnaker-sdk/
### Compiling Dependencies
- Run the Spinnaker SDK installer to extract the Spinaker Library
- Compiling OpenCV library by following instructions in file "Building OpenCV for Windows.pdf" in the doc folder of this repository, or thru [Huan's blog](http://huanktran.com/blog/index.php/2020/05/16/building-opencv-for-windows/)
- Remember for OpenCV, compile/build both the Release and the Debug version
### Generating the project Visual Studio Solution
- From Windows taskbar, Search and Run CMake GUI
- Delete Cache by click on File -> Delete Cache on the top leftcorner
- Redirect the "Where is the source code" box and "Where to build the binaries" box to the folder \HuanGUI\src\Windows\TStracker in this repository
- Hit **Configure**
- If the configuration window pops up, in the first box, select the Visual Studio version that is currently installed in your machine
- The second box should be x64. Then hit OK
- Set the value of NeedToBeSet_OPENCV_DIR to the path where you compile your OpenCV library ( if you follow the tutorial in the doc folder to compile OpenCV, the folder should have been named build_new)
- Set the value of NeedToBeSet_SPINNAKER_DIR to the path where you install your SpinnakerSDK
- After configuring finishes, hit **Generate**
### Build the solution
- After genrating finishes, click the **TStracker.sln** in the TStracker project folder
- Open the **TStracker.sln**  with the visual studio installed in you machine
- Make sure the solution is in **Release** build. You can check this looking at the area close by the menu bar (FILE|EDIT|VIEW|PROJECT, etc), you should see a drop down box with either **Debug** or **Release** inside. If it is in **Debug** click and change to release. Another way to config this is to go to **Build->Configuration Manager** and change **Active solution configuration** to **Release**
- Hit **Build->Build Solution**
- If the solution building encounters error due to missing MFC library, this thread could help [Here](https://stackoverflow.com/a/43075169)
- When building finishes, the folder **Release** should contain the program **TStracker.exe**
- When you run the TStracker.exe and the program complains about missing DLL file of OpenCV library, copy all the *.dll files from TStracker\src\DevelopmentSuite\lib\OpenCVLib4Windows\OpenCV_DLL\ReleaseUseDLL to your System32 folder (normally resides at C:\Windows\System32)