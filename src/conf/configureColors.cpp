#include <sstream>
#include <iostream>
#include <utility>
#include <string>
#include <config.h>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


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


bool showColorConfig(
  cv::VideoCapture &camera, 
  std::pair<std::string, std::string> windows,
  std::string caption,
  HsvRange hsv
) {
  cv::Mat frame, mask;

  camera >> frame;
  hsvMask(frame, mask, hsv);
  cv::putText(
    frame, caption, cv::Point(10, frame.size().height - 10), 
    cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
  );
  cv::imshow(windows.first, frame);
  cv::imshow(windows.second, mask);
  int key = cv::waitKey(10);
  if (key != -1) {
    return false;
  } else {
    return true;
  }
}


std::pair<std::string, std::string> createWindows(std::string colorName, HsvRange &hsv) {
  std::ostringstream cam, mask;
  cam << "Camera: " << colorName;
  mask << "Mask: " << colorName;

  cv::namedWindow(cam.str(), cv::WINDOW_AUTOSIZE);
  cv::createTrackbar("hStart", cam.str(), &(hsv.h.start), 255);
  cv::createTrackbar("hEnd", cam.str(), &(hsv.h.end), 255);
  cv::createTrackbar("sStart", cam.str(), &(hsv.s.start), 255);
  cv::createTrackbar("sEnd", cam.str(), &(hsv.s.end), 255);
  cv::createTrackbar("vStart", cam.str(), &(hsv.v.start), 255);
  cv::createTrackbar("vEnd", cam.str(), &(hsv.v.end), 255);

  cv::namedWindow(mask.str(), cv::WINDOW_AUTOSIZE);
  cv::createTrackbar("erode", mask.str(), &(hsv.erosions), 20);
  cv::createTrackbar("dilate", mask.str(), &(hsv.dilations), 20);

  return std::pair<std::string, std::string>(cam.str(), mask.str());
}


std::string makeCaption(std::string name) {
  std::ostringstream os;
  os << "[" << name << "] Press any key to finish";
  return os.str();
}


void destroyWindows(std::pair<std::string, std::string> windows) {
  cv::destroyWindow(windows.first);
  cv::destroyWindow(windows.second);
}


void configureColor(cv::VideoCapture &camera, std::string name, HsvRange &hsv) {
  std::pair<std::string, std::string> windows = createWindows(name, hsv);
  std::string caption = makeCaption(name);
  while(showColorConfig(camera, windows, caption, hsv)) {}
  destroyWindows(windows);
}


bool yesNoResponse() {
  std::string response;
  std::cin >> response;
  while (response != "y" && response != "n") {
    std::cout << "please type 'y' or 'n'." << std::endl;
    std::cin >> response;
  }
  if (response == "n") {
    return false;
  } else {
    return true;
  }
}


void configureColors(Configuration &config, cv::VideoCapture &camera) {
  for (auto it = config.colors.data.begin(); it != config.colors.data.end(); it++) {
    std::cout << "found color configuration '" << it->first << "'; keep? (y/n)" << std::endl;
    if (yesNoResponse()) {
      configureColor(camera, it->first, it->second);
    } else {
      config.colors.data.erase(it->first);
    }
  }

  bool loop = true;
  do {
    std::cout << "create a new color? (y/n)" << std::endl;
    if (yesNoResponse()) {
      std::cout << "please enter a name for the color" << std::endl;
      std::string name;
      std::cin >> name;
      HsvRange hsv;
      configureColor(camera, name, hsv);
      config.colors.data[name] = hsv;
    } else {
      loop = false;
    }
  } while (loop);
}
