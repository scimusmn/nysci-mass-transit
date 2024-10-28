#pragma once
#include <string>
#include <map>
#include <opencv2/core/persistence.hpp>
#include <opencv2/core.hpp>


struct Serializable {
  void serialize(cv::FileStorage &fs);
  virtual void save(cv::FileStorage &fs) = 0;
  virtual bool load(cv::FileNode &node) = 0;
};

struct Range : public Serializable {
  int start = 0;
  int end = 255;
  void save(cv::FileStorage &fs);
  bool load(cv::FileNode &node);
};

struct HsvRange : public Serializable {
  Range h, s, v;
  int erosions = 0;
  int dilations = 0;
  void save(cv::FileStorage &fs);
  bool load(cv::FileNode &node);
};

struct Colors : public Serializable {
  std::map<std::string, HsvRange> data;
  void save(cv::FileStorage &fs);
  bool load(cv::FileNode &node);
};

struct Calibration : public Serializable {
  cv::Mat cameraMat, distCoeffs;
  void save(cv::FileStorage &fs);
  bool load(cv::FileNode &node);
};

struct CircleDetection : public Serializable {
  double cannyThreshold, accumulatorThreshold, minDist;
  Range radius;
  void save(cv::FileStorage &fs);
  bool load(cv::FileNode &node);
};

struct Configuration : public Serializable {
  int deviceId;
  Calibration calibration;
  CircleDetection circleParams;
  Colors colors;
  void save(cv::FileStorage &fs);
  bool load(cv::FileNode &node);
};


void save(Configuration c, std::string filename);
bool load(Configuration &c, std::string filename);
