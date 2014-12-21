cmake_minimum_required(VERSION 2.8.11)

# Setting project information
set(PROJECT_NAME procal)
project(${PROJECT_NAME})
# Adding C++11
add_definitions(-std=c++11)
include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/macros/SystemSpecificInformations.cmake)

# Setting route to cmake find files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Setting up Qt5 
find_package(Qt5Widgets        REQUIRED)
find_package(Qt5Core           REQUIRED)
find_package(Qt5Gui            REQUIRED)
find_package(Qt5PrintSupport   REQUIRED)
    
set(CMAKE_INCLUDE_CURRENT_DIR  ON)
set(CMAKE_AUTOMOC              ON)

# Compiling Qt5 forms
QT5_WRAP_UI(procal
    ${CMAKE_CURRENT_SOURCE_DIR}/forms/mainwindow.ui 
)

# Setting up project files
file (GLOB Sources              ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file (GLOB Headers              ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file (GLOB UIs                  ${CMAKE_CURRENT_SOURCE_DIR}/forms/*.ui)
file (GLOB Resources            ${CMAKE_CURRENT_SOURCE_DIR}/forms/*.qrc)

# Creating executable and linking 
QT5_ADD_RESOURCES (ResourceSources ${Resources})
add_executable(${PROJECT_NAME} 
		${UIS_HDRS} 
		${UIs} 
		${Sources} 
		${Headers} 
		${ResourceSources})
		
target_link_libraries(${PROJECT_NAME} Qt5::PrintSupport Qt5::Widgets)

# Setting up ZMQ
find_package(ZMQ               REQUIRED)
if(${ZMQ_FOUND})
   message("Using ZMQ headers from: " ${ZMQ_INCLUDE_DIRS})
   include_directories(${ZMQ_INCLUDE_DIRS})
   target_link_libraries(${PROJECT_NAME} ${ZMQ_LIBRARIES})
else()
   message(FATAL "ZMQ not found on this system")
endif()

# Setting up Poco
find_package(Poco               REQUIRED)
if(${Poco_FOUND})
   message("Using Poco librariesfrom: " ${Poco_INCLUDE_DIRS})
   link_directories(${Poco_LIBRARY_DIRS})
   include_directories(${Poco_INCLUDE_DIRS})
   target_link_libraries(${PROJECT_NAME} ${Poco_LIBRARIES})
else()
   message(FATAL "Poco not found on this system")
endif()

INSTALL(TARGETS ${PROJECT_NAME} 
	DESTINATION /usr/local/bin/${PROJECT_NAME})
INSTALL(FILES python/process_analyser.py DESTINATION /usr/local/bin/${PROJECT_NAME})

# Setting up DEB file 
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Renato Samperio")
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Process Analiser")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION 
   "Process analiser and plotter of live memory and cpu usage")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION "Visualisation tool for process analysis")
set(CPACK_PACKAGE_CONTACT "Renato Samperio renatosamperio@gmail.com")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/cmake/control/postinst;${CMAKE_CURRENT_SOURCE_DIR}/cmake/control/preinst;")
# (postinst, preinst, postrm, prerm) 
IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
  SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
ELSEIF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
  SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
ENDIF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
# SET(CPACK_RESOURCE_FILE_LICENSE    "${CMAKE_CURRENT_SOURCE_DIR}/license/Copyright.txt")
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/license/Copyright.txt")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "mesa-common-dev,qt5-default,qttools5-dev-tools,python-setuptools,python-psutil")

# <foo>_<VersionNumber>-<DebianRevisionNumber>_<DebianArchitecture>.deb 
SET(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${DEB_SYSTEM_NAME}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
SET(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
INCLUDE(CPack)
