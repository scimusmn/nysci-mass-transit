#include <random>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <config.h>
#include <processing.h>
#include <main_common.h>


#define BELOW(x, hi) x = x < hi ? x : hi-1;
#define ABOVE(x, lo) x = x > lo ? x : lo;
#define CLAMP(x, lo, hi) do { \
  BELOW(x, hi); \
  ABOVE(x, lo); \
} while(0)


void buildColorMasks(cv::Mat frame, MaskVector &masks, Colors colors) {
  for (auto it = colors.data.begin(); it != colors.data.end(); it++) {
    cv::Mat m;
    hsvMask(frame, m, it->second);
    masks.push_back(std::pair<std::string, cv::Mat>(it->first, m));
  }
}


// get a random point from within the circle
cv::Point randomCirclePoint(cv::Vec3f v, Random &rand) {
  double r = v[2] * sqrt(rand());
  double theta = 2 * 3.141593 * rand();
  int x = v[0] + floor(r * cos(theta));
  int y = v[1] + floor(r * sin(theta));
  return cv::Point(x, y);
}


// determine matching color masks at a given point
void updateColorHistogram(std::vector<int> &histogram, cv::Point p, MaskVector &masks) {
  for (auto it = masks.begin(); it != masks.end(); it++) {
    if (it->second.at<uchar>(p) != 0) {
      auto idx = it - masks.begin();
      histogram[idx] += 1;
    }
  }
}


std::string getCircleColor(cv::Vec3f circle, MaskVector &masks, int iterations, Random &rand) {
  auto size = masks[0].second.size();
  std::vector<int> histogram(masks.size(), 0);
  for (int i=0; i<iterations; i++) {
    cv::Point p = randomCirclePoint(circle, rand);
    CLAMP(p.x, 0, size.width);
    CLAMP(p.y, 0, size.height);
    updateColorHistogram(histogram, p, masks);
  }

  int max = 0;
  int maxIdx = -1;
  for (int i=0; i<histogram.size(); i++) {
    if (histogram[i] > max) {
      max = histogram[i];
      maxIdx = i;
    }
  }

  if (maxIdx < 0) {
    return "?";
  } else {
    return masks[maxIdx].first;
  }
}
