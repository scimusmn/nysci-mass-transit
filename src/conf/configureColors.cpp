#include <sstream>
#include <iostream>
#include <utility>
#include <string>
#include <config.h>
#include <processing.h>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <conf_common.h>


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


void configureColor(cv::VideoCapture &camera, std::string name, HsvRange &hsv) {
  std::ostringstream camwin, maskwin, caption;
  camwin << "Camera: " << name;
  maskwin << "Mask: " << name;
  std::pair<std::string, std::string> windows(
    createWindow(camwin.str(), {
      { "hStart", &(hsv.h.start), 255 },
      { "hEnd", &(hsv.h.end), 255 },
      { "sStart", &(hsv.s.start), 255 },
      { "sEnd", &(hsv.s.end), 255 },
      { "vStart", &(hsv.v.start), 255 },
      { "vEnd", &(hsv.v.end), 255 }
    }),
    createWindow(maskwin.str(), {
      { "erode", &(hsv.erosions), 20 },
      { "dilate", &(hsv.dilations), 20 },
    })
  );
  caption << "[" << name << "] Press any key to finish";
  while(showColorConfig(camera, windows, caption.str(), hsv)) {}
  cv::destroyWindow(windows.first);
  cv::destroyWindow(windows.second);
}


void configureColors(Configuration &config, cv::VideoCapture &camera) {
  std::vector<std::string> eraseKeys;
  for (auto it = config.colors.data.begin(); it != config.colors.data.end(); it++) {
    std::cout << "found color configuration '" << it->first << "'; keep? (y/n): ";
    std::flush(std::cout);
    if (queryYesNo()) {
      std::cout << "edit color? (y/n): ";
      std::flush(std::cout);
      if (queryYesNo()) {
        configureColor(camera, it->first, it->second);
      }
    } else {
      eraseKeys.push_back(it->first);
    }
  }
  for (auto it = eraseKeys.begin(); it != eraseKeys.end(); it++) {
    config.colors.data.erase(*it);
  }

  bool loop = true;
  do {
    std::cout << "create a new color? (y/n): ";
    std::flush(std::cout);
    if (queryYesNo()) {
      std::cout << "please enter a name for the color: ";
      std::flush(std::cout);
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
