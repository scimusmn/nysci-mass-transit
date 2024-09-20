#include "common.h"
#include <DeviceEnumerator.h>
#include <opencv2/core.hpp>


int getVideoDevice(cv::FileStorage& fs) {
  printf("Please select a camera device to use from the options below:\n");
  DeviceEnumerator e;
  auto deviceMap = e.getVideoDevicesMap();
  for (auto it = deviceMap.begin(); it != deviceMap.end(); it++) {
    printf("  %d. %s\n", it->first, it->second.deviceName);
  }
  printf("use device: ");
  fflush(stdout);
  int device = -1;
  while (device < 0) {
    int n = scanf("%d", &device);
    if (n == 0 || deviceMap.find(device) == deviceMap.end()) {
      printf("invalid device, please try again.\n");
      device = -1;
    }
  }

  fs << "device" << "{";
  fs << "number" << device;
  fs << "}";

  return device;
}