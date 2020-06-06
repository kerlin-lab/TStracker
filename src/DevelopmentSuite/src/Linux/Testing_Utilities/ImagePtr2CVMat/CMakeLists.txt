project(ImagePtr2CVMat)
cmake_minimum_required(VERSION 2.8)

# Open CV setting
find_package(OpenCV REQUIRED)
include_directories($(OpenCV_INCLUDE_DIRS))

# Spinnaker setting
include_directories(/usr/include/spinnaker)
set(SpinView_LIBS "Spinnaker_C" "Spinnaker" "SpinUpdate" "SpinVideo_C" "SpinVideo")
# Build
add_executable(ImagePtr2CVMat main.cpp)
target_link_libraries(ImagePtr2CVMat ${OpenCV_LIBS})
target_link_libraries(ImagePtr2CVMat ${SpinView_LIBS})
install(TARGETS ImagePtr2CVMat DESTINATION build)
