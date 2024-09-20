cmake_minimum_required(VERSION 3.8)

project(mass-transit)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
set(OpenCV_DIR "C:\\Users\\kaswanson_smm\\work\\lib\\cpp\\opencv\\out\\install\\x64-Debug")
find_package(OpenCV REQUIRED)
add_executable(
  test 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lines.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/calibration.cpp
)
target_include_directories(test PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test PUBLIC ${OpenCV_LIBS})


configure_file("calibration.xml" ".")
