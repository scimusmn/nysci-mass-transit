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
}
bool HsvRange::load(cv::FileNode &n) {
  return
    unserialize(n, "h", &h) &&
    unserialize(n, "s", &s) &&
    unserialize(n, "v", &v);
}
