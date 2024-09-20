#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include "calibration.h"


bool Calibration::load(const char *filename) {
  cv::FileStorage file(filename, cv::FileStorage::READ);
  if (!file.isOpened()) {
    fprintf(stderr, "failed to open file '%s'\n", filename);
    return false;
  }
  cv::Mat cameraMatrix, distortionCoefficients;
  int width, height;
  file["camera_matrix"] >> cameraMatrix;
  file["distortion_coefficients"] >> distortionCoefficients;
  file["image_width"] >> width;
  file["image_height"] >> height;
  file.release();

  cv::Size imgSize(width, height);
  cv::initUndistortRectifyMap(
    cameraMatrix, distortionCoefficients, cv::Mat(),
    cv::getOptimalNewCameraMatrix(cameraMatrix, distortionCoefficients, imgSize, 1, imgSize, 0), imgSize,
    CV_16SC2, map1, map2
  );
  return true;
}


void Calibration::apply(cv::Mat &frame) {
  cv::remap(frame, frame, map1, map2, cv::INTER_LINEAR);
}
