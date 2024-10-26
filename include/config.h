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
  int start, end;
  void save(cv::FileStorage &fs);
  bool load(cv::FileNode &node);
};

struct HsvRange : public Serializable {
  Range h, s, v;
  void save(cv::FileStorage &fs);
  bool load(cv::FileNode &node);
};
