## installing opencv on windows

1. download the current opencv source code
2. open in Visual Studio and build+install via cmake
3. ADD THE OUTPUT bin/ TO PATH!!
4. in the CMakeLists.txt file of your project, set the OpenCV_DIR variable to the output folder
5. done, hopefully

p.s. opening a camera as CAP_DSHOW can fix the horrible slowness in opening a camera
