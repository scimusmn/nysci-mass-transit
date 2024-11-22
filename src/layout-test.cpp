#include <iostream>

struct tile_t {
  uint16_t x;
  uint16_t y;
  uint8_t type;
};


int main() {
  std::cout << "x: " << offsetof(struct tile_t, x) << std::endl;
  std::cout << "y: " << offsetof(struct tile_t, y) << std::endl;
  std::cout << "type: " << offsetof(struct tile_t, type) << std::endl;
  std::cout << "Total size: " << sizeof(struct tile_t) << std::endl;
  return 0;
}
