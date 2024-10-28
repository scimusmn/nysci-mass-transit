#include <iostream>
#include <conf_common.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


std::string createWindow(std::string name, std::vector<Trackbar> bars) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  for (auto it = bars.begin(); it != bars.end(); it++) {
    cv::createTrackbar(it->name, name, it->ptr, it->max);
  }
  return name;
}


bool queryYesNo() {
  std::string str;
  std::cin >> str;
  while (str != "y" && str != "n") {
    std::cout << "please type 'y' or 'n'." << std::endl;
    std::cin >> str;
  }
  if (str == "y") {
    return true;
  } else {
    return false;
  }
}
