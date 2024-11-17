#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>


#define DICT cv::aruco::PredefinedDictionaryType::DICT_APRILTAG_16h5


void dumpMarker(int idx) {
  std::cout << "dump " << idx << std::endl;
  cv::Mat img;
  cv::aruco::Dictionary dict = cv::aruco::getPredefinedDictionary(DICT);
  cv::aruco::generateImageMarker(dict, idx, 200, img);
  std::string filename = "marker" + std::to_string(idx) + ".png";
  cv::imwrite(filename, img);
}


int main(int argc, char **argv) {
  for (int i=0; i<30; i++) {
    dumpMarker(i);
  }
  return 0;
}
