#include "common.h"
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>


#define CALIBRATION_WIN "Calibration"

#define BOARD_WIDTH 9
#define BOARD_HEIGHT 6



// query how many images to capture
int getImagesCount() {
  printf("how many calibration images to capture: ");
  fflush(stdout);

  int num = -1;
  while(scanf("%d", &num) != 1 || num < 1) {
    printf("Please enter a number greater than 0: ");
    fflush(stdout);
  }

  return num;
}



// loop showing images until a key is pressed
cv::Mat captureCalibrationImage(cv::VideoCapture &camera, int num, int total) {
  cv::Mat frame;
  char buf[200];
  snprintf(buf, 200, "[%d/%d] Press any key to capture image", num, total);

  while(true) {
    camera >> frame;
    cv::putText(
      frame, buf, cv::Point(10, frame.size().height - 10), 
      cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
    );
    cv::imshow(CALIBRATION_WIN, frame);
    int key = cv::waitKey(10);
    if (key != -1) {
      return frame;
    }
  }
}



// get the camera's frame size
cv::Size getImageSize(cv::VideoCapture &camera) {
  double width = camera.get(cv::CAP_PROP_FRAME_WIDTH);
  double height = camera.get(cv::CAP_PROP_FRAME_HEIGHT);
  return cv::Size(width, height);
}

// compute the chessboard corner locations
void computeChessboardCorners(std::vector<cv::Point3f> &points, cv::Size boardSize) {
  for (int y=0; y < boardSize.height; y++) {
    for (int x=0; x < boardSize.width; x++) {
      points.push_back(cv::Point3f(x, y, 0));
    }
  }
}

// get calibration data from an image
bool captureCalibrationData(
  cv::VideoCapture &camera,
  cv::Size boardSize,
  std::vector<std::vector<cv::Point3f>> &objectPoints,
  std::vector<std::vector<cv::Point2f>> &imagePoints,
  int num, int total
) {
  cv::Mat frame = captureCalibrationImage(camera, num, total);
  std::vector<cv::Point2f> points;
  if (cv::findChessboardCorners(frame, boardSize, points)) {
    std::vector<cv::Point3f> corners;
    computeChessboardCorners(corners, boardSize);
    objectPoints.push_back(corners);
    imagePoints.push_back(points);
    return true;
  } else {
    // no board detected :C
    return false;
  }
}



// collect data & compute calibration 
void calibrateCamera(
  cv::FileStorage &fs,
  cv::VideoCapture &camera, int count, cv::Size boardSize
) {
  std::vector<std::vector<cv::Point3f>> objectPoints;
  std::vector<std::vector<cv::Point2f>> imagePoints;
  int captures = 0;
  while (captures < count) {
    if (captureCalibrationData(
      camera, boardSize, objectPoints, imagePoints, captures, count
    )) {
      captures += 1;
    }
  }

  // compute calibration
  auto newObjectPoints = objectPoints[0];
  cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
  cv::Mat distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
  std::vector<cv::Mat> rvecs, tvecs;
  cv::calibrateCameraRO(
    objectPoints, imagePoints, getImageSize(camera), -1,
    cameraMatrix, distCoeffs, rvecs, tvecs, newObjectPoints
  );

  // save calibration
  fs << NODE_CALIBRATION << "{";
  fs << "camera_matrix" << cameraMatrix;
  fs << "distortion_coefficients" << distCoeffs;
  fs << "}";
}



// perform image calibration
void calibrate(cv::FileStorage& fs, cv::VideoCapture &camera) {
  int num = getImagesCount();
  printf("capturing %d images...\n", num);

  // create window and calibrate
  cv::namedWindow(CALIBRATION_WIN, cv::WINDOW_AUTOSIZE);
  calibrateCamera(fs, camera, num, cv::Size(BOARD_WIDTH, BOARD_HEIGHT));
  printf("calibration successful!\n");
}
