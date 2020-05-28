# Project name
set(ProjName "TStracker")
set(CMAKE_GENERATOR_PLATFORM x64 CACHE INTERNAL "")

## Directory config (need to be modified accodingly to each machine). These variables seem to be redundent but
## will help the GUI version of CMAKE display the variables nicely. If you are modifying manually, change the value of these variables
## directly in this file

set(NeedToBeSet_SPINNAKER_DIR CACHE PATH "Spinnaker library folder")
set(NeedToBeSet_OPENCV_DIR CACHE PATH "OpenCV library folder")


## Reassigning the directory

# Spinnaker directory
set(SPINNAKER_DIR ${NeedToBeSet_SPINNAKER_DIR})

# OpenCV directory
set(OpenCV_DIR ${NeedToBeSet_OPENCV_DIR})

# LibTiff directory
set(LIBTIFF_INC_DIR "${OpenCV_DIR}/../sources/3rdparty/libtiff;${OpenCV_DIR}/3rdparty/libtiff")


## Creating project and associating dependencies
project(${ProjName})
cmake_minimum_required(VERSION 2.8)

# TStracker Source code
set(SOURCE_FILE
	./main.cpp  
	./TStracker.cpp 
	./TStrackerMFC.cpp
	./TStrackerThread.cpp
	./ImageInfo.cpp
	./ImageSaver.cpp)

set(HEADER_FILE
	./cvui.h
	./resource.h
	./TStracker.h
	./TStrackerMFC.h
	./TStrackerThread.h
	./stdafx.h
	./ImageInfo.h
	./ImageSaver.h)

# Open CV setting
find_package(OpenCV REQUIRED)					# Link OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Libtiff and 3rdParty lib setting
include_directories(${LIBTIFF_INC_DIR})
link_directories("${NeedToBeSet_OPENCV_DIR}/3rdparty/lib/Release")
set(SpinView_LIBS "AdapterConfig_v140" "Spinnaker_v140" "SpinnakerC_v140" "SpinnakerCd_v140" "Spinnakerd_v140" "SpinnakerGUI_WPF_v140" "SpinnakerGUI_WPFd_v140" "SpinnakerGUISelector_WPF_v140" "SpinUpdate_v140" "SpinVideo_v140" "SpinVideoC_v140" "SpinVideoCd_v140" "SpinVideod_v140")


# Spinnaker setting
include_directories("${SPINNAKER_DIR}/include")
link_directories("${SPINNAKER_DIR}/lib64/vs2015")
set(LibTIFF_LIBS	
	"libpng"
	"libtiff"
	"zlib"
	"IlmImf"
	"libjasper"
	"libjpeg-turbo")


### TStracker setting

## Setting library uses
set(CMAKE_MFC_FLAG 2)	# Turn on MFC
add_executable(${ProjName} WIN32 ${SOURCE_FILE} ${HEADER_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/rc/${ProjName}.rc)
# Linking library
target_link_libraries(${ProjName}  ${OpenCV_LIBS})
target_link_libraries(${ProjName} ${SpinView_LIBS})
target_link_libraries(${ProjName} ${LibTIFF_LIBS})
# Output the project configuration
install(TARGETS ${ProjName} DESTINATION ./)

