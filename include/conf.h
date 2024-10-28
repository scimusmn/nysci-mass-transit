#pragma once

#include <opencv2/videoio.hpp>
#include <config.h>


void configureVideoDevice(Configuration &config);
void configureCalibration(Configuration &config, cv::VideoCapture &camera);
void configureColors(Configuration &config, cv::VideoCapture &camera);
void configureCircleParams(Configuration &config, cv::VideoCapture &camera);
