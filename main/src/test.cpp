#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <stdio.h>


int main(int argc, char **argv) {
  printf("args: %d\n", argc);
  printf("showing image '%s'\n", argv[1]);
  cv::Mat img = cv::imread(argv[1], cv::IMREAD_COLOR);

  cv::imshow("picture", img);
  int k = cv::waitKey(0);
  return 0;
}
