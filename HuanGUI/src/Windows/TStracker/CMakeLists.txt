# Project name
set(ProjName "TStracker")
set(CMAKE_GENERATOR_PLATFORM x64 CACHE INTERNAL "")

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
include_directories($(OpenCV_INCLUDE_DIRS))

# Spinnaker setting
set(SPINNAKER_DIR "D:/Software/Programming_software/External_Library/Spinnaker")
include_directories("${SPINNAKER_DIR}/include")
link_directories("${SPINNAKER_DIR}/lib64/vs2015")
set(SpinView_LIBS "AdapterConfig_v140" "Spinnaker_v140" "SpinnakerC_v140" "SpinnakerCd_v140" "Spinnakerd_v140" "SpinnakerGUI_WPF_v140" "SpinnakerGUI_WPFd_v140" "SpinnakerGUISelector_WPF_v140" "SpinUpdate_v140" "SpinVideo_v140" "SpinVideoC_v140" "SpinVideoCd_v140" "SpinVideod_v140")

# TStracker setting

## Setting library uses
set(CMAKE_MFC_FLAG 2)	# Turn on MFC
add_executable(${ProjName} WIN32 ${SOURCE_FILE} ${HEADER_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/rc/${ProjName}.rc)
target_link_libraries(${ProjName}  ${OpenCV_LIBS})
target_link_libraries(${ProjName} ${SpinView_LIBS})
install(TARGETS ${ProjName} DESTINATION ./)

