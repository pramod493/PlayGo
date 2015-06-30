cmake_minimum_required(VERSION 3.0.2)
project (QsLog)

#set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR})
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(QSLOG_SRCS
${PROJECT_SOURCE_DIR}/QsLog.cpp
${PROJECT_SOURCE_DIR}/QsLogDest.cpp
${PROJECT_SOURCE_DIR}/QsDebugOutput.cpp
)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

add_library(${PROJECT_NAME} SHARED ${QSLOG_SRCS})