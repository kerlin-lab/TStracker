# How to compile the project
## Software needs:
- CMAKE GUI
- Visual Studio (Any version)
- OpenCV 4.3.0 library
- Spinnaker SDK from FLIR camera version 1.29.0.5_x64 (Or any version supporting Visual Studio 2015 toolchain v140)
## Compiling Dependencies
- Run the Spinnaker SDK installer to extract the Spinaker Library
- Compiling OpenCV library by following instructions in file "Building OpenCV for Windows.pdf" in the doc folder of this repository, or thru [Huan's blog](http://huanktran.com/blog/index.php/2020/05/16/building-opencv-for-windows/)
## Generating the project Visual Studio Solution
- Run CMake GUI
- Redirect the "Where is the source code" box and "Where to build the binaries" box to the folder \HuanGUI\src\Windows\TStracker in this repository
- Hit **Configure**
- If the configuration window pops up, in the first box, select the Visual Studio version that is currently installed in your machine
- The second box should be x64. Then hit OK
- After configuring finishes, hit **Generate**
## Build the solution
- After genrating finishes, you should **TStracker.sln** in the TStracker project folder
- Open the **TStracker.sln**  with the visual studio installed in you machine
- Make sure the solution is in **Release** build. You can check this looking at the area close by the menu bar (FILE|EDIT|VIEW|PROJECT, etc), you should see a drop down box with either **Debug** or **Release** inside. If it is in **Debug** click and change to release. Another way to config this is to go to **Build->Configuration Manager** and change **Active solution configuration** to **Release**
- Hit **Build->Build Solution**
- When building finishes, the folder **x64/Release** should contain the program **TStracker.exe**