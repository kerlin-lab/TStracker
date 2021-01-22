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
- Redirect the "Where is the source code" box and "Where to build the binaries" box to the folder \src\TStracker in this repository
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
## Common issues
### I was able to compile the program but TStracker complains about missing DLL file of OpenCV library when being executed
- There are 2 ways to resolve this
#### Solution 1 - Adding the path to the compiled OpenCV lib to the system environment PATH variable
- Hit Windows key -> search "Edit the system environment variables"
- A window pops up, click on Environemt Variables
- Under the "User variables for "Your user name", click on Path and hit Edit
- Hit New and add a new entry with the path pointing to the bin\Release and bin\Debug folders of your compiled OpenCV library
- Normally they look like C:\OpenCV\build_new\bin\Debug and C:\OpenCV\build_new\bin\Release
- Then hit Ok -> Ok -> Ok and and close the popped up in Window
- You should be able to run TStracker now, if it stills complains about the missing DLL, try restart the computer and try again. Or try the following solution
#### Solution 2 - Copy the dll manually
- Copy all the *.dll files from src\DevelopmentSuite\lib\OpenCVLib4Windows\OpenCV_DLL\ReleaseUseDLL in this repository to your System32 folder (normally resides at C:\Windows\System32)
- There is an install.bat file in the folder to help you do this, right click on it and choose run as Administrator.
- When you want to remove TStracker, run the uninstall.bat as Adminstrator