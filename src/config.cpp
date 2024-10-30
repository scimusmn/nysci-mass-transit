#include <iostream>
#include <config.h>
#include <string>
#include <map>
#include <opencv2/core.hpp>


void Serializable::serialize(cv::FileStorage &fs) {
  fs << "{";
  save(fs);
  fs << "}";
}


template<typename T>
bool unserialize(cv::FileNode &n, std::string key, T &v) {
  cv::FileNode nn = n[key];
  if (nn.isNone()) {
    return false;
  } else {
    nn >> v;
    return true;
  }
}
bool unserialize(cv::FileNode &parent, std::string key, Serializable *s) {
  if (parent.isNone()) {
    return false;
  }
  cv::FileNode n = parent[key];
  if (n.isNone()) {
    // node does not exist
    return false;
  } else {
    return s->load(n);
  }
}



void Range::save(cv::FileStorage &fs) {
  fs << "start" << start;
  fs << "end" << end;
}
bool Range::load(cv::FileNode &n) {
  return 
    unserialize(n, "start", start) &&
    unserialize(n, "end", end);
}


void HsvRange::save(cv::FileStorage &fs) {
  fs << "h"; h.serialize(fs);
  fs << "s"; s.serialize(fs);
  fs << "v"; v.serialize(fs);
  fs << "erosions" << erosions;
  fs << "dilations" << dilations;
}
bool HsvRange::load(cv::FileNode &n) {
  return
    unserialize(n, "h", &h) &&
    unserialize(n, "s", &s) &&
    unserialize(n, "v", &v) &&
    unserialize(n, "erosions", erosions) &&
    unserialize(n, "dilations", dilations);
}


void Colors::save(cv::FileStorage &fs) {
  for (auto it = data.begin(); it != data.end(); it++) {
    fs << it->first; it->second.serialize(fs);
  }
}
bool Colors::load(cv::FileNode &node) {
  HsvRange r;
  std::vector<std::string> keys = node.keys();
  for (auto it = node.begin(); it != node.end(); it++) {
    std::cout << (*it).name() << std::endl;
    r.load(*it);
    data[(*it).name()] = r;
  }
  return true;
}


void Calibration::save(cv::FileStorage &fs) {
  fs << "cameraMat" << cameraMat;
  fs << "distCoeffs" << distCoeffs;
}
bool Calibration::load(cv::FileNode &n) {
  return
    unserialize(n, "cameraMat", cameraMat) &&
    unserialize(n, "distCoeffs", distCoeffs);
}


void CircleDetection::save(cv::FileStorage &fs) {
  fs << "cannyThreshold" << cannyThreshold;
  fs << "accumulatorThreshold" << accumulatorThreshold;
  fs << "minDist" << minDist;
  fs << "radius"; radius.serialize(fs);
}
bool CircleDetection::load(cv::FileNode &n) {
  return
    unserialize(n, "cannyThreshold", cannyThreshold) &&
    unserialize(n, "accumulatorThreshold", accumulatorThreshold) &&
    unserialize(n, "minDist", minDist) &&
    unserialize(n, "radius", &radius);
}


void Configuration::save(cv::FileStorage &fs) {
  if (width > 0) {
    fs << "width" << width;
  }
  if (height > 0) {
     fs << "height" << height;
  }
  fs << "deviceId" << deviceId;
  fs << "calibration"; calibration.serialize(fs);
  fs << "colors"; colors.serialize(fs);
  fs << "projection" << projection;
  fs << "circleParams"; circleParams.serialize(fs);
}
bool Configuration::load(cv::FileNode &n) {
  if (!unserialize(n, "width", width)) {
    width = -1;
  }
  if (!unserialize(n, "height", height)) {
    height = -1;
  }
  return
    unserialize(n, "deviceId", deviceId) &&
    unserialize(n, "calibration", &calibration) &&
    unserialize(n, "colors", &colors) &&
    unserialize(n, "projection", projection) &&
    unserialize(n, "circleParams", &circleParams);
}


void save(Configuration c, std::string filename) {
  cv::FileStorage fs(filename, cv::FileStorage::WRITE);
  fs << "config"; c.serialize(fs);
  fs.release();
}
bool load(Configuration &c, std::string filename) {
  try {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    bool result = c.load(fs["config"]);
    fs.release();
    return result;
  } catch(...) {
    return false;
  }
}


void openCamera(cv::VideoCapture &camera, Configuration &c) {
  camera.open(c.deviceId, cv::CAP_DSHOW);
  if (c.width > 0) {
    camera.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, c.width);
  }
  if (c.height > 0) {
    camera.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, c.height);
  }

}
