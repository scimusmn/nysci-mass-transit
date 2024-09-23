#include "common.h"
#include <cargs.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>


#define DEFAULT_CONF_FILENAME "config.xml"


struct cag_option options[] = {
  { 'o', "o", NULL, "OUTPUT FILENAME", "output config file (default " DEFAULT_CONF_FILENAME ")" },
  { 'd', "d", NULL, NULL, "pick the video device to use" },
  { 'c', "c", NULL, NULL, "calibrate the camera" },
  { 't', "t", NULL, NULL, "set color thresholds" },
  { 'h', "h", "help", NULL, "display help message" },
};


struct actions {
  bool getVideoDevice = false;
  bool calibrate = false;
  bool setThresholds = false;
};


void separate() {
  printf("\n================================\n\n");
}



int main(int argc, char **argv) {
  // parse command line options
  const char *filename = DEFAULT_CONF_FILENAME;
  struct actions action;
  bool actionFlag = false;
  cag_option_context context;
  cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    switch(cag_option_get_identifier(&context)) {
      case 'o':
        filename = cag_option_get_value(&context);
        break;
      case 'd':
        action.getVideoDevice = true;
        break;
      case 'c':
        action.calibrate = true;
        break;
      case 't':
        action.setThresholds = true;
        break;
      case 'h':
        printf("Usage: configure-vision [OPTION]...\n", argv[0]);
        printf("Generate a configuration file for mass transit vision\n");
        cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
        return 0;
      case '?':
        cag_option_print_error(&context, stdout);
        return 1;
    }
  }

  printf("\noutputting to %s\n", filename);
  separate();
  cv::FileStorage fs;
  int device = -1;
  try {
    fs.open(filename, cv::FileStorage::READ);

    // check for configurations that are empty
    if (fs[NODE_VIDEO_DEVICE].isNone()) {
      action.getVideoDevice = true;
    }
    else { 
      fs[NODE_VIDEO_DEVICE] >> device;
    }
    if (fs[NODE_CALIBRATION].isNone()) { action.calibrate = true; }
    if (fs[NODE_THRESHOLDS].isNone()) { action.setThresholds = true; }
    fs.release();
  }
  catch (...) {
    // the file does not exist; do all actions
    memset(&action, true, sizeof(struct actions));
  }

  fs.open(filename, cv::FileStorage::APPEND);

  // create requested configurations
  if (action.getVideoDevice) { device = getVideoDevice(fs); separate(); }
  cv::VideoCapture camera;
  if (action.calibrate || action.setThresholds) { camera.open(device, cv::CAP_DSHOW); }
  if (!camera.isOpened()) {
    fprintf(stderr, "FATAL: could not open camera!\n");
    return 1;
  }
  if (action.calibrate) { calibrate(fs, camera); separate(); }
  if (action.setThresholds) { setThresholds(fs, camera); separate(); }

  fs.release();
  printf("Configuration complete!\n\n");
  return 0;
}
