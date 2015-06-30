project(PlayGo)
cmake_minimum_required(VERSION 3.0.2)

# Find the QtWidgets library
find_package(Qt5Widgets)

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
target_link_libraries(PlayGo PlayGo_lib)

qt5_use_modules(PlayGo_lib Core Gui Widgets)

add_subdirectory(QsLog)