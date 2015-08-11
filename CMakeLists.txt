project(PlayGo)
cmake_minimum_required(VERSION 3.0.2)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)


# Find the QtWidgets library
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(Qt5Sensors)
find_package(Qt5PrintSupport REQUIRED)

set (PlayGo_src ${PROJECT_SOURCE_DIR}/miscfunctions.cpp ${PROJECT_SOURCE_DIR}/converttopolygons.cpp)
set (PlayGo_hdr ${PROJECT_SOURCE_DIR}/converttopolygons.h)

# No UI file present in this project
# The gesture recognizer uses the UI files
#set (PlayGo_ui   ${PROJECT_SOURCE_DIR}/qm2cm.ui)

set (PlayGo_bin_src ${PROJECT_SOURCE_DIR}/main.cpp)

qt5_wrap_cpp(PlayGo_hdr_moc ${PlayGo_hdr})
#qt5_wrap_ui (PlayGo_ui_moc ${PlayGo_ui})

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

add_library(PlayGo_lib SHARED
        ${PlayGo_src}
        ${PlayGo_hdr_moc}
        )
# add ${PlayGo_ui_moc} to include UIs

add_executable(PlayGo ${PlayGo_bin_src})
target_link_libraries(PlayGo PlayGo_lib
						Qt5::Widgets Qt5::Multimedia Qt5::Gui Qt5::Network)

qt5_use_modules(PlayGo_lib Core Gui Widgets)

add_subdirectory(QsLog)