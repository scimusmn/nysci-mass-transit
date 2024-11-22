cmake_minimum_required(VERSION 3.8)

project(mass-transit)

# modify to point to your OpenCV install as necessary
find_package(OpenCV REQUIRED PATHS C:\\tools\\opencv\\build)

message("opencv include path: ${OpenCV_INCLUDE_DIRS}")


# files shared across the configuration tool and main program
set(COMMON_CPP
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cargs/cargs.c
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/DeviceEnumerator/DeviceEnumerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/processing.cpp
)
set(COMMON_INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cargs/
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/DeviceEnumerator/
  ${OpenCV_INCLUDE_DIRS}
)

add_executable(
  configure
  ${CMAKE_CURRENT_SOURCE_DIR}/src/conf/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/conf/common.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/conf/configureVideoDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/conf/configureCalibration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/conf/configureColors.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/conf/configureCircleParams.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/conf/configureProjection.cpp
  ${COMMON_CPP}
)
target_include_directories(configure PUBLIC ${COMMON_INCLUDE})
target_link_libraries(configure PUBLIC ${OpenCV_LIBS})
add_executable(
  mt-input
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/color.cpp
  ${COMMON_CPP}
)
target_include_directories(mt-input PUBLIC ${COMMON_INCLUDE})
target_link_libraries(mt-input PUBLIC ${OpenCV_LIBS})

add_executable(
  runegen
  ${CMAKE_CURRENT_SOURCE_DIR}/src/runegen.cpp
  ${COMMON_CPP}
)
target_include_directories(runegen PUBLIC ${COMMON_INCLUDE})
target_link_libraries(runegen PUBLIC ${OpenCV_LIBS})


add_executable(layout-test ${CMAKE_CURRENT_SOURCE_DIR}/src/layout-test.cpp)


configure_file("config.xml.example" "config.xml")
