cmake_minimum_required(VERSION 2.6)
 
set(PROJECT NewEditor)
 
project(${PROJECT})

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
 
set(HEADERS src/NewEditor.h src/MainWindow.h src/Settings.h src/DatSprOpenDialog.h src/DatSprStructs.h src/DatSprReaderWriter.h)

set(SOURCES src/NewEditor.cpp src/MainWindow.cpp src/Settings.cpp src/DatSprOpenDialog.cpp src/DatSprReaderWriter.cpp)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-deprecated-declarations -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -D__DEBUG__")
set(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "release")

find_package(wxWidgets)
include(${wxWidgets_USE_FILE})
 
add_executable (${PROJECT} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${wxWidgets_LIBRARIES})
