#include <stdio.h>
#include <config.h>
#include <conf_common.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>


void findCircles(std::vector<cv::Vec3f> &circles, cv::Mat &frame, CircleDetection params) {
  cv::HoughCircles(
    frame, circles, cv::HOUGH_GRADIENT,
    1, params.minDist, params.cannyThreshold, params.accumulatorThreshold,
    params.radius.start, params.radius.end
  );
}


void drawCircles(std::vector<cv::Vec3f> &circles, cv::Mat &render) {
  for (auto it = circles.begin(); it != circles.end(); it++) {
    cv::circle(render, cv::Point((*it)[0], (*it)[1]), (*it)[2], cv::Scalar(0, 255, 0));
  }
}


bool showCirclesConfig(
  cv::VideoCapture &camera, 
  std::string window,
  CircleDetection params,
  cv::Mat projection
) {
  cv::Mat frame, gray;
  camera >> frame;
  cv::warpPerspective(frame, frame, projection, frame.size());
  cv::cvtColor(frame, gray, cv::ColorConversionCodes::COLOR_BGR2GRAY);
  std::vector<cv::Vec3f> circles;
  findCircles(circles, gray, params);
  drawCircles(circles, frame);
  cv::putText(
    frame, "Press any key to finish", cv::Point(10, frame.size().height - 10), 
    cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
  );
  cv::imshow(window, frame);
  int key = cv::waitKey(10);
  if (key != -1) {
    return false;
  } else {
    return true;
  }
}


void configureCircleParams(Configuration &config, cv::VideoCapture &camera) {
  std::string window = createWindow(
    "Canny circle parameters",
    {
      { "Edge Sens", &(config.circleParams.cannyThreshold), 300 },
      { "C. Sens", &(config.circleParams.accumulatorThreshold), 300 },
      { "Min Dist", &(config.circleParams.minDist), 300 },
      { "Min Radius", &(config.circleParams.radius.start), 300 },
      { "Max Radius", &(config.circleParams.radius.end), 300 }
    }
  );
  while (showCirclesConfig(camera, window, config.circleParams, config.projection)) {}
  cv::destroyWindow(window);
}
