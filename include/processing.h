#pragma once

#include <config.h>
#include <opencv2/core.hpp>


void hsvMask(cv::Mat &frame, cv::Mat &mask, HsvRange hsv);
