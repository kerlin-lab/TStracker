set(ProjName "BasicGUITimeStamp")
project(${ProjName})
cmake_minimum_required(VERSION 2.8)

# Open CV setting
find_package(OpenCV REQUIRED)
include_directories($(OpenCV_INCLUDE_DIRS))

# Spinnaker setting
include_directories(D:/Software/Programming_software/External_Library/Spinnaker/include)
link_directories(D:/Software/Programming_software/External_Library/Spinnaker/lib64/vs2015)
set(SpinView_LIBS "AdapterConfig_v140" "Spinnaker_v140" "SpinnakerC_v140" "SpinnakerCd_v140" "Spinnakerd_v140" "SpinnakerGUI_WPF_v140" "SpinnakerGUI_WPFd_v140" "SpinnakerGUISelector_WPF_v140" "SpinUpdate_v140" "SpinVideo_v140" "SpinVideoC_v140" "SpinVideoCd_v140" "SpinVideod_v140")
# Build
add_executable(${ProjName} main.cpp)
target_link_libraries(${ProjName}  ${OpenCV_LIBS})
target_link_libraries(${ProjName} ${SpinView_LIBS})
install(TARGETS ${ProjName} DESTINATION ./)
