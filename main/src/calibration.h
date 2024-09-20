#pragma once

#include <opencv2/core.hpp>


struct Calibration {
  cv::Mat map1, map2;
  bool load(const char *filename);
  void apply(cv::Mat &frame);
};
