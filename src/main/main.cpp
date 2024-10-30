#include <random>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <config.h>
#include <processing.h>
#include <cargs.h>
#include <iostream>


#define DEFAULT_CONFIG_FILE "config.xml"


struct cag_option options[] = {
  { 'f', "f", NULL, "CONFIG_FILE", "config file to read (default " DEFAULT_CONFIG_FILE ")" },
  { 'v', "v", "visualize", NULL, "visually display the processing" },
  { 'h', "h", "help", NULL, "display help message" },
};


#define BELOW(x, hi) x = x < hi ? x : hi-1;
#define ABOVE(x, lo) x = x > lo ? x : lo;
#define CLAMP(x, lo, hi) do { \
  BELOW(x, hi); \
  ABOVE(x, lo); \
} while(0)



typedef std::vector<std::pair<std::string, cv::Mat>> MaskVector;

struct Random {
  std::default_random_engine e;
  std::uniform_real_distribution<> dist;
  double operator()() {
    return dist(e);
  };
};


void buildColorMasks(cv::Mat frame, MaskVector &masks, Colors colors) {
  for (auto it = colors.data.begin(); it != colors.data.end(); it++) {
    cv::Mat m;
    hsvMask(frame, m, it->second);
    std::cout << "mat type: " << m.type() << std::endl;
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
      std::cout << "color match: " << it->first << std::endl;
      auto idx = it - masks.begin();
      histogram[idx] += 1;
      std::cout << idx << ": " << histogram[idx] << std::endl;
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


void loop(cv::VideoCapture &camera, Configuration &config, Random &rand, bool visualize) {
  cv::Mat frame, gray;
  camera >> frame;
  cv::warpPerspective(frame, frame, config.projection, frame.size());
  cv::cvtColor(frame, gray, cv::ColorConversionCodes::COLOR_BGR2GRAY);

  MaskVector masks;
  buildColorMasks(frame, masks, config.colors);
  if (visualize) {
    for (auto it = masks.begin(); it != masks.end(); it++) {
      cv::imshow(it->first, it->second);
    }
  }

  std::vector<cv::Vec3f> circles;
  findCircles(circles, gray, config.circleParams);
  std::vector<std::string> circleColors;
  for (auto it = circles.begin(); it != circles.end(); it++) {
    circleColors.push_back(getCircleColor(*it, masks, 50, rand));
    if (visualize) {
      cv::circle(frame, cv::Point((*it)[0], (*it)[1]), (*it)[2], CV_RGB(0, 255, 0));
      cv::putText(
        frame, circleColors[circleColors.size()-1], cv::Point((*it)[0], (*it)[1]),
        cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
      );
    }
  }

  if (visualize) {
    cv::imshow("camera", frame);
    cv::waitKey(1);
  }
}



int main(int argc, char **argv) {
  Random rand;
  Configuration config;
  const char *filename = DEFAULT_CONFIG_FILE;
  bool visualize = false;
  
  // parse command-line options
  cag_option_context ctx;
  cag_option_init(&ctx, options, CAG_ARRAY_SIZE(options), argc, argv);
  while(cag_option_fetch(&ctx)) {
    switch(cag_option_get_identifier(&ctx)) {
      case 'f':
        filename = cag_option_get_value(&ctx);
        break;
      case 'v':
        visualize = true;
        break;
      case 'h':
        printf("Usage: mt-input [OPTION]...\n");
        printf("Use computer vision to control a transit simulation in real-time.\n");
        cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
        return 0;
      case '?':
        cag_option_print_error(&ctx, stdout);
        return 1;
    }
  }

  if (!load(config, filename)) {
    std::cerr << "error when reading file '" << filename << "'; aborting!" << std::endl;
    return 1;
  }


  cv::VideoCapture camera;
  openCamera(camera, config);
    
  while(true) {
    loop(camera, config, rand, visualize);
  }

  camera.release();
  return 0;
};
