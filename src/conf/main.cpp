#include <config.h>
#include <conf_common.h>
#include <cargs.h>
#include <stdio.h>


#define DEFAULT_CONF_FILENAME "config.xml"


struct cag_option options[] = {
  { 'f', "f", NULL, "OUTPUT FILENAME", "output config file (default " DEFAULT_CONF_FILENAME ")" },
  { 'd', "d", NULL, NULL, "pick the video device to use" },
  { 'c', "c", NULL, NULL, "calibrate the camera" },
  { 't', "t", NULL, NULL, "set color thresholds" },
  { 'p', "p", NULL, NULL, "set circle detection parameters" },
  { 'h', "h", "help", NULL, "display help message" },
};


struct Actions {
  bool videoDevice = false;
  bool calibrate = false;
  bool colors = false;
  bool circles = false;
};


void printSeparator() {
  printf("\n================================================================\n\n");
}


int main(int argc, char **argv) {
  Configuration config;
  Actions actions;
  const char *filename = DEFAULT_CONF_FILENAME;
  
  // parse command-line options
  cag_option_context ctx;
  cag_option_init(&ctx, options, CAG_ARRAY_SIZE(options), argc, argv);
  while(cag_option_fetch(&ctx)) {
    switch(cag_option_get_identifier(&ctx)) {
      case 'f':
        filename = cag_option_get_value(&ctx);
        break;
      case 'd':
        actions.videoDevice = true;
        break;
      case 'c':
        actions.calibrate = true;
        break;
      case 't':
        actions.colors = true;
        break;
      case 'p':
        actions.circles = true;
        break;
      case 'h':
        printf("Usage: configure [OPTION]...\n");
        printf("Generate a configuration file for NYSCI MASS TRANSIT VISION\n");
        cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
        return 0;
      case '?':
        cag_option_print_error(&ctx, stdout);
        return 1;
    }
  }


  printf("\nusing config file: %s\n", filename);
  bool acted = false;
  if (!load(config, filename)) {
    memset(&actions, true, sizeof(struct Actions));
  }

  printSeparator();

  if (actions.videoDevice) {
    acted = true;
    configureVideoDevice(config);
    printSeparator();
  }

  cv::VideoCapture camera(config.deviceId, cv::CAP_DSHOW);

  if (actions.calibrate) {
    acted = true;
    configureCalibration(config, camera);
    printSeparator();
  }
  if (actions.colors) {
    acted = true;
    configureColors(config, camera);
    printSeparator();
  }
  if (actions.circles) {
    acted = true;
    configureCircleParams(config, camera);
    printSeparator();
  }

  camera.release();

  if (acted) {
    save(config, filename);
  }

  return 0;
}
