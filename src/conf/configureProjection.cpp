#include <vector>
#include <sstream>
#include <stdio.h>
#include <config.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>



#define MARK_RADIUS 3
#define MARK_COLOR cv::Scalar(0, 255, 0)



void mouseCb(int event, int x, int y, int flags, void *userdata) {
  auto v = reinterpret_cast<std::vector<cv::Point2f>*>(userdata);
  switch(event) {
    case cv::MouseEventTypes::EVENT_LBUTTONDOWN:
      // left click
      v->push_back(cv::Point2f(x, y));
      break;
    case cv::MouseEventTypes::EVENT_RBUTTONDOWN:
      // right click
      if (v->size() > 0) {
        v->pop_back();
      }
      break;
    default:
      // ignore
      break;
  }
}



void getProjectionPoints(cv::VideoCapture &camera, std::string window, std::vector<cv::Point2f> &v) {
  cv::Mat frame;
  camera >> frame;
  for (auto it = v.begin(); it != v.end(); it++) {
    cv::circle(frame, cv::Point(it->x, it->y), MARK_RADIUS, MARK_COLOR);
  }
  std::ostringstream caption;
  caption << "Click on the ";
  switch(v.size()) {
    case 0:
      caption << "UPPER LEFT ";
      break;
    case 1:
      caption << "UPPER RIGHT ";
      break;
    case 2:
      caption << "LOWER LEFT ";
      break;
    case 3:
      caption << "LOWER RIGHT ";
      break;
    default:
      caption << "????? ";
      break;
  }
  caption << "corner; right click to undo";
  cv::putText(
    frame, caption.str(), cv::Point(10, frame.size().height - 10), 
    cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
  );
  cv::imshow(window, frame);
  cv::waitKey(10);
}


int previewProjection(cv::VideoCapture &camera, std::string window, cv::Mat projection) {
  cv::Mat frame;
  camera >> frame;
  cv::warpPerspective(frame, frame, projection, frame.size());
  cv::putText(
    frame, "Press any key to finish; right click to return to point selection", cv::Point(10, frame.size().height - 10),
    cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
  );
  cv::imshow(window, frame);
  int result = cv::waitKey(10);
  if (result != -1) {
    return true;
  } else {
    return false;
  }
}


cv::Mat loop(cv::VideoCapture &camera, std::string window, std::vector<cv::Point2f> &v) {
  while (v.size() < 4) {
    getProjectionPoints(camera, window, v);
  }

  float w = camera.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH);
  float h = camera.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT);

  std::vector<cv::Point2f> expected = {
    { 0, 0 }, { w, 0 },
    { 0, h }, { w, h },
  };
  cv::Mat projection = cv::getPerspectiveTransform(v, expected);
  std::cout << projection << std::endl;

  while (true) {
    if (previewProjection(camera, window, projection)) {
      return projection;
    } else if (v.size() < 4) {
      return loop(camera, window, v);
    }
  }
}


// perform image calibration
void configureProjection(Configuration &config, cv::VideoCapture &camera) {
  std::vector<cv::Point2f> v;
  std::string window = "Project surface";
  cv::namedWindow(window, cv::WINDOW_AUTOSIZE);
  cv::setMouseCallback(window, mouseCb, &v);

  // get projection points
  config.projection = loop(camera, window, v);
  cv::destroyWindow(window);
}
