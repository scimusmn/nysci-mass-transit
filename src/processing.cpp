#include <vector>
#include <config.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

void rangeMask(cv::Mat &mat, Range range) {
  cv::Mat low;
  if (range.start < range.end) {
    cv::threshold(mat, low, range.start, 0xff, cv::THRESH_BINARY);
    cv::threshold(mat, mat, range.end,   0xff, cv::THRESH_BINARY_INV);
  } else {
    cv::threshold(mat, low, range.end,   0xff, cv::THRESH_BINARY_INV);
    cv::threshold(mat, mat, range.start, 0xff, cv::THRESH_BINARY);
  }
  cv::bitwise_and(mat, low, mat);
}


void hsvMask(cv::Mat &frame, cv::Mat &mask, HsvRange hsv) {
  cv::Mat mat;
  cv::cvtColor(frame, mat, cv::COLOR_BGR2HSV);
  std::vector<cv::Mat> channels;
  cv::split(mat, channels);

  rangeMask(channels[0], hsv.h);
  rangeMask(channels[1], hsv.s);
  rangeMask(channels[2], hsv.v);
  cv::bitwise_and(channels[1], channels[0], mask);
  cv::bitwise_and(channels[2], mask, mask);

  cv::Mat kernel = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(3, 3));
  cv::Point pt(-1, -1);
  cv::erode(mask, mask, kernel, pt, hsv.erosions);
  cv::dilate(mask, mask, kernel, pt, hsv.dilations);
}


void findCircles(std::vector<cv::Vec3f> &circles, cv::Mat &frame, CircleDetection params) {
  cv::HoughCircles(
    frame, circles, cv::HOUGH_GRADIENT,
    1, params.minDist, params.cannyThreshold, params.accumulatorThreshold,
    params.radius.start, params.radius.end
  );
}


