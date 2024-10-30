#pragma once

#include <config.h>
#include <opencv2/core.hpp>


void hsvMask(cv::Mat &frame, cv::Mat &mask, HsvRange hsv);

void findCircles(std::vector<cv::Vec3f> &circles, cv::Mat &frame, CircleDetection params);
