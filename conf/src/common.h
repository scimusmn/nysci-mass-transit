#pragma once
#include <opencv2/core.hpp>


// device enumeration
int getVideoDevice(cv::FileStorage& fs);

// calibration
void calibrate(cv::FileStorage& fs, int camera);
