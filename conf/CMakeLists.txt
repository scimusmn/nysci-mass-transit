cmake_minimum_required(VERSION 3.8)

project(mass-transit)

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
set(OpenCV_DIR "C:\\Users\\kaswanson_smm\\work\\lib\\cpp\\opencv\\out\\install\\x64-Debug")
find_package(OpenCV REQUIRED)
add_executable(
  configure-vision
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/deviceEnumeration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/calibrate.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/thresholds.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/features.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/DeviceEnumerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cargs.c
  )
target_include_directories(configure-vision PUBLIC 
  ${OpenCV_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
  )
target_link_libraries(configure-vision PUBLIC ${OpenCV_LIBS})
