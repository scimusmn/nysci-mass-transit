#pragma once

#include <random>
#include <string>
#include <opencv2/core.hpp>


struct Random {
  std::default_random_engine e;
  std::uniform_real_distribution<> dist;
  double operator()() {
    return dist(e);
  };
};


typedef std::vector<std::pair<std::string, cv::Mat>> MaskVector;


void buildColorMasks(cv::Mat frame, MaskVector &masks, Colors colors);
std::string getCircleColor(cv::Vec3f circle, MaskVector &masks, int iterations, Random &rand);
