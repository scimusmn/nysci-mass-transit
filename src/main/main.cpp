#include <chrono>
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
  { 'c', "c", "colors", NULL, "visually display the color processing" },
  { 'h', "h", "help", NULL, "display help message" },
};


struct Visualization {
  bool main;
  bool colors;
};


#define GRID_W 10
#define GRID_H 10
#define BUMP_RATE 3
#define DRAIN_RATE 1


struct PointScore {
  double x, y;
  unsigned int score = 0;
};
struct Grid {
  std::vector<PointScore> x[GRID_W][GRID_H];
  std::vector<PointScore> * operator[](int idx) {
    return x[idx];
  }
};


cv::Point gridSquare(PointScore p) {
  cv::Point pt;
  pt.x = floor(GRID_W * p.x);
  pt.y = floor(GRID_H * p.y);
  return pt;
}


void insertPoint(Grid &grid, PointScore p) {
  cv::Point square = gridSquare(p);
  grid[square.x][square.y].push_back(p);
}

std::vector<PointScore> elementsAt(Grid &grid, cv::Point square) {
  if (
    (square.x >= 0 && square.x < GRID_W) &&
    (square.y >= 0 && square.y < GRID_H)
  ) {
    return grid[square.x][square.y];
  } else {
    return std::vector<PointScore>(0);
  }
}


double dist2(PointScore a, PointScore b) {
  double dx = a.x - b.x;
  double dy = a.y - b.y;
  return (dx*dx) + (dy*dy);
}


void updateCircle(Grid &grid, PointScore p, double r) {
  bool match = false;
  double r2 = r*r;
  cv::Point square = gridSquare(p);
  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      cv::Point s = { square.x + dx, square.y + dy };
      auto e = elementsAt(grid, s);
      for (auto it = e.begin(); it != e.end(); it++) {
        if (dist2(*it, p) < r2) {
          it->score += BUMP_RATE;
          match = true;
        }
      }
    }
  }

  if (!match) {
    p.score = BUMP_RATE;
    insertPoint(grid, p);
  }
}


void updateGrid(Grid &grid) {
  for (int x = 0; x<GRID_W; x++) {
    for (int y = 0; y<GRID_H; y++) {
      auto &e = grid[x][y];
      for (auto it = e.begin(); it != e.end();) {
        it->score -= DRAIN_RATE;
        if (it->score <= 0) {
          it = e.erase(it);
        } else {
          it++;
        }
      }
    }
  }
}





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


void loop(cv::VideoCapture &camera, Configuration &config, Random &rand, Visualization viz) {
  auto startTime = std::chrono::high_resolution_clock::now();
  cv::Mat frame, gray;
  camera >> frame;
  cv::warpPerspective(frame, frame, config.projection, frame.size());
  cv::cvtColor(frame, gray, cv::ColorConversionCodes::COLOR_BGR2GRAY);

  MaskVector masks;
  buildColorMasks(frame, masks, config.colors);
  if (viz.colors) {
    for (auto it = masks.begin(); it != masks.end(); it++) {
      cv::imshow(it->first, it->second);
    }
  }

  std::vector<cv::Vec3f> circles;
  findCircles(circles, gray, config.circleParams);
  std::vector<std::string> circleColors;
  for (auto it = circles.begin(); it != circles.end(); it++) {
    circleColors.push_back(getCircleColor(*it, masks, 50, rand));
    if (viz.main) {
      cv::circle(frame, cv::Point((*it)[0], (*it)[1]), (*it)[2], CV_RGB(0, 255, 0));
      cv::putText(
        frame, circleColors[circleColors.size()-1], cv::Point((*it)[0], (*it)[1]),
        cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
      );
    }
  }

  if (viz.main) {
    cv::imshow("camera", frame);
    cv::waitKey(1);
  }
  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
  std::cout << "processed frame in " << duration.count() << " ms" << std::endl;
}



int main(int argc, char **argv) {
  Random rand;
  Configuration config;
  const char *filename = DEFAULT_CONFIG_FILE;
  Visualization viz = { false, false };
  
  // parse command-line options
  cag_option_context ctx;
  cag_option_init(&ctx, options, CAG_ARRAY_SIZE(options), argc, argv);
  while(cag_option_fetch(&ctx)) {
    switch(cag_option_get_identifier(&ctx)) {
      case 'f':
        filename = cag_option_get_value(&ctx);
        break;
      case 'v':
        viz.main = true;
        break;
      case 'c':
        viz.colors = true;
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
    loop(camera, config, rand, viz);
  }

  camera.release();
  return 0;
};
