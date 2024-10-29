#pragma once

#include <opencv2/videoio.hpp>
#include <config.h>


void configureVideoDevice(Configuration &config);
void configureCalibration(Configuration &config, cv::VideoCapture &camera);
void configureColors(Configuration &config, cv::VideoCapture &camera);
void configureCircleParams(Configuration &config, cv::VideoCapture &camera);
void configureProjection(Configuration &config, cv::VideoCapture &camera);


struct Trackbar {
  std::string name;
  int *ptr;
  int max;
};


typedef std::pair<std::string, std::string> WindowPair;


std::string createWindow(std::string name, std::vector<Trackbar> bars);
bool queryYesNo();
