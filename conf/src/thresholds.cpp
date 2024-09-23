#include "common.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>


#define FG_WINDOW_NAME "Foreground Thresholds"
#define BG_WINDOW_NAME "Background Thresholds"


// save a range to disk
void storeRange(cv::FileStorage& fs, const char *name, struct range_t r) {
  fs << name << "{";
  fs << "min" << r.a;
  fs << "max" << r.b;
  fs << "}";
}


// load a set of color thresholds from disk
void loadThresholds(cv::FileNode& node, struct thresholds_t& tr) {
  node["hue"]["min"] >> tr.hue.a;
  node["hue"]["max"] >> tr.hue.b;
  node["saturation"]["min"] >> tr.saturation.a;
  node["saturation"]["max"] >> tr.saturation.b;
  node["value"]["min"] >> tr.value.a;
  node["value"]["max"] >> tr.value.b;
}


// create a mask from a value range
void rangeMask(cv::Mat& mat, struct range_t range) {
  cv::Mat lowMask;
  if (range.a < range.b) {
    cv::threshold(mat, lowMask, range.a, 0xff, cv::THRESH_BINARY);
    cv::threshold(mat, mat,     range.b, 0xff, cv::THRESH_BINARY_INV);
  } else {
    cv::threshold(mat, lowMask,  range.b, 0xff, cv::THRESH_BINARY_INV);
    cv::threshold(mat, mat,      range.a, 0xff, cv::THRESH_BINARY);
  }
  cv::bitwise_and(mat, lowMask, mat);
}


// create a mask from HSV ranges
void thresholdMask(cv::Mat& mat, cv::Mat& mask, struct thresholds_t tr) {
  cv::Mat hsv;
  cv::cvtColor(mat, hsv, cv::COLOR_BGR2HSV);
  std::vector<cv::Mat> channels;
  cv::split(hsv, channels);

  rangeMask(channels[0], tr.hue);
  rangeMask(channels[1], tr.saturation);
  rangeMask(channels[2], tr.value);
  cv::bitwise_and(channels[1], channels[0], mask);
  cv::bitwise_and(channels[2], mask, mask);
  cv::threshold(mask, mask, 1, 1, cv::THRESH_BINARY);
}


void applyMask(cv::Mat& frame, cv::Mat& mask) {
  std::vector<cv::Mat> channels;
  cv::split(frame, channels);

  channels[0] = channels[0].mul(mask);
  channels[1] = channels[1].mul(mask);
  channels[2] = channels[2].mul(mask);

  cv::merge(channels, frame);
}


bool showThresholds(cv::VideoCapture& camera, struct thresholds_t tr, const char *windowName) {
  cv::Mat frame, mask, masked;
  camera >> frame;
  thresholdMask(frame, mask, tr);
  applyMask(frame, mask);
  cv::putText(
    frame, "Press any key to finish", cv::Point(10, frame.size().height - 10),
    cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
  );
  cv::imshow(windowName, frame);
  if (cv::waitKey(10) != -1) {
    return false;
  }
  return true;
}



void createWindow(const char *name, struct thresholds_t& tr) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  cv::createTrackbar("hue_min", name, &(tr.hue.a), 255);
  cv::createTrackbar("hue_max", name, &(tr.hue.b), 255);

  cv::createTrackbar("sat_min", name, &(tr.saturation.a), 255);
  cv::createTrackbar("sat_max", name, &(tr.saturation.b), 255);

  cv::createTrackbar("val_min", name, &(tr.value.a), 255);
  cv::createTrackbar("val_max", name, &(tr.value.b), 255);
}


void setFgThresholds(cv::FileStorage& fs, cv::VideoCapture& camera) {
  struct thresholds_t tr;
  createWindow(FG_WINDOW_NAME, tr);
  while(showThresholds(camera, tr, FG_WINDOW_NAME)) {
    // loopy
  }

  fs << "fg" << "{";
  storeRange(fs, "hue",        tr.hue);
  storeRange(fs, "saturation", tr.saturation);
  storeRange(fs, "value",      tr.value);
  fs << "}";
}


void setBgThresholds(cv::FileStorage& fs, cv::VideoCapture& camera) {
  struct thresholds_t tr;
  createWindow(BG_WINDOW_NAME, tr);
  while(showThresholds(camera, tr, BG_WINDOW_NAME)) {
    // loopy
  }

  fs << "bg" << "{";
  storeRange(fs, "hue",        tr.hue);
  storeRange(fs, "saturation", tr.saturation);
  storeRange(fs, "value",      tr.value);
  fs << "}";
}


void setThresholds(cv::FileStorage& fs, cv::VideoCapture& camera) {
  fs << NODE_THRESHOLDS << "{";
  setFgThresholds(fs, camera);
  setBgThresholds(fs, camera);
  fs << "}";
}
