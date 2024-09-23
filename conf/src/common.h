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
struct range_t {
  int a = 0;
  int b = 255;
};
struct thresholds_t {
  struct range_t hue;
  struct range_t saturation;
  struct range_t value;
};
void thresholdMask(cv::Mat& mat, cv::Mat& mask, struct thresholds_t tr);
void applyMask(cv::Mat& frame, cv::Mat& mask);
void loadThresholds(cv::FileNode& node, struct thresholds_t& tr);
#define NODE_THRESHOLDS "thresholds"
void setThresholds(cv::FileStorage& fs, cv::VideoCapture& camera);


// features 
#define NODE_FEATURES "features"
void setFeatures(cv::FileStorage& fs, cv::VideoCapture& camera);
