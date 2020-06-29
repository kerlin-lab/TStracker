# How to compile the project
## Software needs:
- CMAKE GUI
- Visual Studio (Any version)
- OpenCV 4.3.0 library
- Spinnaker SDK from FLIR camera version 1.29.0.5_x64 (Or any version supporting Visual Studio 2015 toolchain v140)
## Downloading compiling tools
- Get CMAKE GUI at cmake.org
- Get OpenCV at opencv.org/releases/
- Get spinnaker sdk as flir.com/products/spinnaker-sdk/
## Compiling Dependencies
- Run the Spinnaker SDK installer to extract the Spinaker Library
- Compiling OpenCV library by following instructions in file "Building OpenCV for Windows.pdf" in the doc folder of this repository, or thru [Huan's blog](http://huanktran.com/blog/index.php/2020/05/16/building-opencv-for-windows/)
- Remember for OpenCV, compile/build both the Release and the Debug version
## Generating the project Visual Studio Solution
- From Windows taskbar, Search and Run CMake GUI
- Delete Cache by click on File -> Delete Cache on the top leftcorner
- Redirect the "Where is the source code" box and "Where to build the binaries" box to the folder \HuanGUI\src\Windows\TStracker in this repository
- Hit **Configure**
- If the configuration window pops up, in the first box, select the Visual Studio version that is currently installed in your machine
- The second box should be x64. Then hit OK
- Set the value of NeedToBeSet_OPENCV_DIR to the path where you compile your OpenCV library ( if you follow the tutorial in the doc folder to compile OpenCV, the folder should have been named build_new)
- Set the value of NeedToBeSet_SPINNAKER_DIR to the path where you install your SpinnakerSDK
- After configuring finishes, hit **Generate**
## Build the solution
- After genrating finishes, click the **TStracker.sln** in the TStracker project folder
- Open the **TStracker.sln**  with the visual studio installed in you machine
- Make sure the solution is in **Release** build. You can check this looking at the area close by the menu bar (FILE|EDIT|VIEW|PROJECT, etc), you should see a drop down box with either **Debug** or **Release** inside. If it is in **Debug** click and change to release. Another way to config this is to go to **Build->Configuration Manager** and change **Active solution configuration** to **Release**
- Hit **Build->Build Solution**
- If the solution building encounters error due to missing MFC library, this thread could help [Here](https://stackoverflow.com/a/43075169)
- When building finishes, the folder **Release** should contain the program **TStracker.exe**
- When you run the TStracker.exe and the program complains about missing DLL file of OpenCV library, copy all the *.dll files from TStracker\src\DevelopmentSuite\lib\OpenCVLib4Windows\OpenCV_DLL\ReleaseUseDLL to your System32 folder (normally resides at C:\Windows\System32)