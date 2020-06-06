# Quick Config for new project
set(ProjName "OpenCV_QT_GUI")	# Setting a variable

# Config CMAKE, don't touch anything below this line
project(${ProjName})
cmake_minimum_required(VERSION 2.8)

# Load OpenCV packages 
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Config build for executable
add_executable(${ProjName} main.cpp)
target_link_libraries(${ProjName} ${OpenCV_LIBS})
