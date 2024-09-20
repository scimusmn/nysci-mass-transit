#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <vector>
#include <stdio.h>

#include "calibration.h"


using namespace cv;


#define CANNY_HIGH_MAX 500
struct CannyParams {
	int low = 0;
	int ratio = 3;
	int kernelSize = 3;
};


#define DIST_MIN 100 
#define DIST_MAX 500 
struct CircleParams {
  int minDist = 30;
  int cannyHigh = 200;
  int accThresh = 100;
  int minRadius = 0;
  int maxRadius = 0;
};


const char* cannyWindow = "Canny Lines";
const char* circleWindow = "Hough Circles";


void canny(Mat& frame, Mat& out, struct CannyParams p) {
	blur(frame, out, Size(3, 3));
	Canny(frame, out, p.low, p.low * p.ratio, p.kernelSize);
}


void findCircles(std::vector<Vec3f> &circles, Mat &frame, CircleParams p) {
  HoughCircles(
    frame, circles, cv::HOUGH_GRADIENT,
    1, p.minDist, p.cannyHigh, p.accThresh, 
    p.minRadius, p.maxRadius
  );
}


void drawCircles(std::vector<Vec3f> &circles, Mat &render) {
  for (auto v = circles.begin(); v != circles.end(); v++) {
    cv::circle(render, cv::Point((*v)[0], (*v)[1]), (*v)[2], 0xff0000);
  }
}


int loop(VideoCapture& camera, Calibration calib, CannyParams pCanny, CircleParams pHough) {
	Mat frame, edges;
	// capture image
	camera >> frame;
	cvtColor(frame, frame, COLOR_BGR2GRAY);
	calib.apply(frame);
	// detect edges
	canny(frame, edges, pCanny);
	imshow(cannyWindow, edges);
	// detect circles 
  std::vector<Vec3f> circles;
  findCircles(circles, frame, pHough);
  drawCircles(circles, frame);
	imshow(circleWindow, frame);
	// wrap up
	if (waitKey(10) == 27) {
		// escape pressed
		return 0;
	} else {
		return 1;
	}
}


int main(int argc, char **argv) {
  Calibration calib;
  if (!calib.load("calibration.xml")) {
    return 1;
  }

	VideoCapture camera(0, CAP_DSHOW);
	if (!camera.isOpened()) {
		fprintf(stderr, "FATAL: could not open camera!\n");
		return 1;
	}

  struct CannyParams canny;
  struct CircleParams hough;

  namedWindow(cannyWindow, WINDOW_AUTOSIZE);
  createTrackbar("lowthresh", cannyWindow, &(canny.low), CANNY_HIGH_MAX);

	namedWindow(circleWindow, WINDOW_AUTOSIZE);
	createTrackbar("thresh", circleWindow, &(hough.cannyHigh), CANNY_HIGH_MAX);
	createTrackbar("minDist", circleWindow, &(hough.minDist), DIST_MIN);
	createTrackbar("accThres", circleWindow, &(hough.accThresh), 500);
	createTrackbar("minRadius", circleWindow, &(hough.minRadius), 300);
	createTrackbar("maxRadius", circleWindow, &(hough.maxRadius), 300);

	while (loop(camera, calib, canny, hough)) {}

	return 0;
}
