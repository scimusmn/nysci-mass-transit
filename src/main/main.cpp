#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <config.h>
#include <config.h>
#include <processing.h>
#include <cargs.h>
#include <iostream>
#include <main_common.h>


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


struct ScorePoint {
  double x, y;
  int score;
};


#define SCORE_UP 3
#define SCORE_DOWN 1
#define SCORE_MAX 20 
#define SCORE_THRESH 10


double dist2(ScorePoint a, ScorePoint b) {
  double dx = a.x - b.x;
  double dy = a.y - b.y;
  return (dx*dx) + (dy*dy);
}


void insertPoint(std::vector<ScorePoint> &points, ScorePoint p, double r) {
  double r2 = r*r;
  bool match = false;
  for (auto it=points.begin(); it != points.end(); it++) {
    double d = dist2(*it, p);
    if (dist2(*it, p) < r2) {
      it->score += SCORE_UP;
      if (it->score > SCORE_MAX) {
        it->score = SCORE_MAX;
      }
      match = true;
    }
  }
  if (!match) {
    p.score = SCORE_UP;
    points.push_back(p);
  }
}


void updatePoints(std::vector<ScorePoint> &points) {
  for (auto it=points.begin(); it != points.end();) {
    it->score -= SCORE_DOWN;
    if (it->score < 0) {
      it = points.erase(it);
    } else {
      it++;
    }
  }
}


void loop(
  cv::VideoCapture &camera, 
  Configuration &config,
  Random &rand,
  std::map<std::string, std::vector<ScorePoint>> &points,
  Visualization viz
) {
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
  for (auto it = circles.begin(); it != circles.end(); it++) {
    std::string color = getCircleColor(*it, masks, 50, rand);
    double x = (*it)[0];
    double y = (*it)[1];
    double r = (*it)[2];
    insertPoint(points[color], { x, y, 0 }, r);
    // if (viz.main) {
    //   cv::circle(frame, cv::Point(x, y), r, CV_RGB(0, 255, 0));
    //   cv::putText(
    //     frame, color, cv::Point(x, y),
    //     cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
    //   );
    // }
  }

  for (auto it=points.begin(); it != points.end(); it++) {
    updatePoints(it->second);
    if (viz.main) {
      std::string color = it->first;
      for (auto jt = it->second.begin(); jt != it->second.end(); jt++) {
        if (jt->score > SCORE_THRESH) {
          std::ostringstream str;
          str << color << ": " << jt->score;
          cv::circle(frame, cv::Point(jt->x, jt->y), config.circleParams.radius.start, CV_RGB(0, 255, 0));
          cv::putText(
            frame, str.str(), cv::Point(jt->x, jt->y),
            cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0)
          );
        }
      }
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
  
  std::map<std::string, std::vector<ScorePoint>> points;
  for (auto it = config.colors.data.begin(); it != config.colors.data.end(); it++) {
    points[it->first] = {};
  }

  cv::VideoCapture camera;
  openCamera(camera, config);
    
  while(true) {
    loop(camera, config, rand, points, viz);
  }

  camera.release();
  return 0;
};
