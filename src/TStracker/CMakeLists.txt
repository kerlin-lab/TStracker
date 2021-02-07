# Project name
set(ProjName "TStracker")
# Set this compiler to x64
set(CMAKE_GENERATOR_PLATFORM "x64")
message(WARNING "Target Platform has been changed to x64, this project cannot be compiled with x86 compiler (this is a warining so don't panic)")

## Directory config (need to be modified accodingly to each machine). These variables seem to be redundent but
## will help the GUI version of CMAKE display the variables nicely. If you are modifying manually, change the value of these variables
## directly in this file

set(NeedToBeSet_SPINNAKER_DIR "" CACHE PATH "Spinnaker library folder")
set(NeedToBeSet_OPENCV_DIR "" CACHE PATH "OpenCV library folder")

if(NeedToBeSet_SPINNAKER_DIR STREQUAL "" OR NeedToBeSet_OPENCV_DIR} STREQUAL "")
	# if the two variable above has not been set yet then output errror to let user set them
	message( FATAL_ERROR "Please provide the path to Spinnaker and OpenCV library")
endif()

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
set(SOURCE_FILE				# This is what appears in the File manager when sort the TStracker folder by type
	./CamRecorder.cpp
	./CVDisplay.cpp
	./ImageDistributor.cpp
	./ImageInfo.cpp
	./ImageMiner.cpp
	./ImageSaverTSQ.cpp
	./InputDialog.cpp
	./main.cpp  
	./RunOperator.cpp
	./TiffWriter.cpp
	./TSImage.cpp
	./TStracker.cpp 
	./TStrackerMFC.cpp
	./TStrackerThread.cpp
	./TSRecordingSetting.cpp
	./TSSettingDialog.cpp
	./utils.cpp)

set(HEADER_FILE				# This is what appears in the File manager when sort the TStracker folder by type
	./rc/resource.h
	./CamRecorder.h
	./CVDisplay.h
	./cvui.h
	./ImageDistributor.h
	./ImageInfo.h
	./ImageMiner.h
	./ImageSaverTSQ.h
	./InputDialog.h
	./RunOperator.h
	./ThreadSafeQueue.h
	./ThreadSafeVariable.h
	./TiffWriter.h
	./TSImage.h
	./TStracker.h
	./TStrackerMFC.h
	./TStrackerThread.h
	./stdafx.h
	./TStrackerType.h
	./TSRecordingSetting.h
	./TSSettingDialog.h
	./utils.h)

# Open CV setting
find_package(OpenCV PATHS ${OpenCV_DIR})					# Link OpenCV
# Only proceed if the path to OpenCV library is correct
if (NOT ${OpenCV_FOUND})
	# Preventing OpenCV_DIR variable appears in the GUI setting, which can confuse user
	unset(OpenCV_DIR CACHE)
	# Output error message and stop
	message(FATAL_ERROR "Cannot locate OpenCV library with given path. Did you point the path to folder myBuild or to folder you built your OpenCV lib?")
endif()
# link the include directory
include_directories(${OpenCV_INCLUDE_DIRS})

# Libtiff and 3rdParty lib setting
include_directories(${LIBTIFF_INC_DIR})
link_directories("${NeedToBeSet_OPENCV_DIR}/3rdparty/lib/Release")
set(SpinView_LIBS "AdapterConfig_v140" "Spinnaker_v140" "SpinnakerC_v140" "SpinnakerCd_v140" "Spinnakerd_v140" "SpinnakerGUI_WPF_v140" "SpinnakerGUI_WPFd_v140" "SpinnakerGUISelector_WPF_v140" "SpinUpdate_v140" "SpinVideo_v140" "SpinVideoC_v140" "SpinVideoCd_v140" "SpinVideod_v140")


# Spinnaker setting
# check if user gave correct path to spinnaker lib
if(NOT EXISTS "${SPINNAKER_DIR}/lib64/vs2015")
	message(FATAL_ERROR "Cannot locate library folder in given path to Spinnaker library, please enter the correct path")
endif()

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

