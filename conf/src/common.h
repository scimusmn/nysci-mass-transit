#pragma once
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>


// device enumeration
#define NODE_VIDEO_DEVICE "video_device"
int getVideoDevice(cv::FileStorage& fs);

// calibration
#define NODE_CALIBRATION "calibration"
void calibrate(cv::FileStorage& fs, cv::VideoCapture &camera);

// thresholds
#define NODE_THRESHOLDS "thresholds"
void setThresholds(cv::FileStorage& fs, cv::VideoCapture& camera);
