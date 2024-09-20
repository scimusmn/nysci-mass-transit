#include <stdio.h>
#include <cargs.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <DeviceEnumerator.h>


#define DEFAULT_CONF_FILENAME "transit.conf"



struct cag_option options[] = {
  { .identifier = 'o',
    .access_letters = "o",
    .access_name = NULL,
    .value_name = "OUTPUT FILENAME",
    .description = "output config file (default " DEFAULT_CONF_FILENAME ")",
  },
  { .identifier = 'h',
    .access_letters = "h",
    .access_name = "help",
    .value_name = NULL,
    .description = "output config file (default " DEFAULT_CONF_FILENAME ")",
  },
};


int main(int argc, char **argv) {
  // parse command line options
  const char *filename = DEFAULT_CONF_FILENAME;
  cag_option_context context;
  cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    switch(cag_option_get_identifier(&context)) {
      case 'o':
        filename = cag_option_get_value(&context);
        break;
      case 'h':
        printf("Usage: %s [OPTION]...\n", argv[0]);
        printf("Generate a configuration file for mass transit vision\n");
        cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
        return 0;
      case '?':
        cag_option_print_error(&context, stdout);
        return 1;
    }
  }

  printf("outputting to %s\n", filename);
  return 0;
}
