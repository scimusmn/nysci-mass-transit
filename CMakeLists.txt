cmake_minimum_required(VERSION 3.8)

project(mass-transit)

# modify to point to your OpenCV install as necessary
set(OpenCV_DIR "C:\\Users\\kaswanson_smm\\work\\lib\\cpp\\opencv\\out\\install\\x64-Debug")
find_package(OpenCV REQUIRED)


# files shared across the configuration tool and main program
set(COMMON_CPP
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cargs.c
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/DeviceEnumerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/processing.cpp
)
set(COMMON_INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
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
target_include_directories(configure PUBLIC ${OpenCV_INCLUDE_DIRS} ${COMMON_INCLUDE})
target_link_libraries(configure PUBLIC ${OpenCV_LIBS})
add_executable(
  mt-input
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/color.cpp
  ${COMMON_CPP}
)
target_include_directories(mt-input PUBLIC ${OpenCV_INCLUDE_DIRS} ${COMMON_INCLUDE})
target_link_libraries(mt-input PUBLIC ${OpenCV_LIBS})

configure_file("config.xml.example" "config.xml")
