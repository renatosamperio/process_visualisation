cmake_minimum_required(VERSION 2.8.11)

project(testproject)

add_definitions(-std=c++11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(ZMQ               REQUIRED)

find_package(Qt5Widgets        REQUIRED)
find_package(Qt5Core           REQUIRED)
find_package(Qt5Gui            REQUIRED)
find_package(Qt5PrintSupport   REQUIRED)
    
set(CMAKE_INCLUDE_CURRENT_DIR  ON)
# set(CMAKE_AUTOMOC              ON)

file (GLOB Sources              *.cpp)
file (GLOB Headers              *.h)
file (GLOB UIs                  *.ui)
file (GLOB Resources            *.qrc)

QT5_ADD_RESOURCES (ResourceSources ${Resources})
add_executable(plot_process_analyser ${Sources} ${Headers} ${ResourceSources})

if(${ZMQ_FOUND})
   message("Using ZNQ headers from: " ${ZMQ_INCLUDE_DIRS})
   include_directories(${ZMQ_INCLUDE_DIRS})
   target_link_libraries(plot_process_analyser ${ZMQ_LIBRARIES})
else()
   message(FATAL "ZMQ not found on this system")
endif()

target_link_libraries(plot_process_analyser Qt5::PrintSupport Qt5::Widgets)