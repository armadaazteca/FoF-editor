cmake_minimum_required(VERSION 2.6)
 
set(PROJECT "FoF-editor")
set(EXECUTABLE_NAME "FoF-editor")

project(${PROJECT})

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
 
set(HEADERS
	src/AboutDialog.h
	src/AdvancedAttributesDialog.h
	src/AdvancedAttributesManager.h
	src/AutoBackupDialog.h
	src/Config.h
	src/DatSprOpenSaveDialog.h
	src/DatSprReaderWriter.h
	src/DatSprStructs.h
	src/Events.h
	src/ExportAnimationDialog.h
	src/FindObjectDialog.h
	src/GenerateRMEDialog.h
	src/Interfaces.h
	src/ItemsOtbWriter.h
	src/MainWindow.h
	src/NewEditor.h
	src/PreferencesDialog.h
	src/QuickGuideDialog.h
	src/Settings.h
	src/SpritesCleanupDialog.h
	src/Utils.h
	src/XmlWriter.h
)

set(SOURCES
	src/AboutDialog.cpp
	src/AdvancedAttributesDialog.cpp
	src/AdvancedAttributesManager.cpp
	src/AutoBackupDialog.cpp
	src/Config.cpp
	src/DatSprOpenSaveDialog.cpp
	src/DatSprReaderWriter.cpp
	src/Events.cpp
	src/ExportAnimationDialog.cpp
	src/FindObjectDialog.cpp
	src/GenerateRMEDialog.cpp
	src/ItemsOtbWriter.cpp
	src/MainWindow.cpp
	src/NewEditor.cpp
	src/PreferencesDialog.cpp
	src/QuickGuideDialog.cpp
	src/Settings.cpp
	src/SpritesCleanupDialog.cpp
	src/Utils.cpp
	src/XmlWriter.cpp
)

if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "GCC detected, adding compile flags")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-deprecated-declarations -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -D__DEBUG__")
	set(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "release")

find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${ImageMagick_Magick++_INCLUDE_DIRS})
 
add_executable (${EXECUTABLE_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} ${wxWidgets_LIBRARIES} ${ImageMagick_LIBRARIES} ${ImageMagick_Magick++_LIBRARIES})
