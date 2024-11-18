#include <cargs.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>


typedef unsigned long long codeword;
uint8_t CODE_BITS = 43;
#define CODE_MAX ((1 << CODE_BITS) - 1)


codeword rotate(codeword c) {
  return ((c << 1) & CODE_MAX) | (c >> (CODE_BITS-1));
}
std::vector<codeword> rotations(codeword c) {
  std::vector<codeword> v;
  for (int i=0; i<CODE_BITS; i++) {
    v.push_back(c);
    c = rotate(c);
  }
  return v;
}

uint8_t popcount8(uint8_t x) {
  uint8_t count = 0;
  for(; x; count++) {
    x &= x-1;
  }
  return count;
}
uint8_t POP8[256];
uint8_t popcount(codeword c) {
  uint8_t count = 0;
  for (int i=0; c; i++) {
     count += POP8[c & 0xff];
     c = c >> (8*i);
  }
  return count;
}
void initPop8() {
  for (unsigned int i=0; i<256; i++) {

  }
}
uint8_t minDist(codeword a, codeword b) {
  auto ra = rotations(a);
  auto rb = rotations(b);
  uint8_t dist = CODE_MAX;
  uint8_t subdist;
  for (auto ca = ra.begin(); ca != ra.end(); ca++) {
    subdist = CODE_MAX;
    for (auto cb = rb.begin(); cb != rb.end(); cb++) {
      subdist = min(subdist, popcount(*ca ^ *cb));
    }
    dist = min(subdist, dist);
  }
  return dist;
}


codeword randomCodeword() {
  uint16_t a, b, c, d;
  a = rand(); b = rand(); c = rand(); d = rand();
  return (a << 48) | (b << 32) | (c << 16) | d;
}


struct cag_option options[] = {
  { 'b', "b", "bits", "CODE_BITS", "number of bits per code" },
  { 'd', "d", "distance", "MIN_DIST", "minimum Hamming distance between each codeword" },
  { 'c', "c", "count", "COUNT", "size of the dictionary to generate" },
  { 'h', "h", "help", NULL, "show this help message" },
};



int main(int argc, char **argv) {
  uint8_t dist = 20;
  uint8_t count = 32;
  cag_option_context ctx;
  cag_option_init(&ctx, options, CAG_ARRAY_SIZE(options), argc, argv);
  while(cag_option_get_identifier(&ctx)) {
    switch(cag_option_get_identifier(&ctx)) {
    case 'b':
      CODE_BITS = std::stoi(cag_option_get_value(&ctx));
      if (CODE_BITS > 64) {
        std::cerr << "cannot specify more than 64 bits per codeword!" << std::endl;
        return 1;
      }
      break;
    case 'd':
      dist = std::stoi(cag_option_get_value(&ctx));
      break;
    case 'c':
      count = std::stoi(cag_option_get_value(&ctx));
      break;
    case 'h':
      std::cout << "Usage: runegen [OPTION]..." << std::endl;
      std::cout << "Generate pseudo-RUNETAG codewords for small marker sets" << std::endl;
      cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
      return 0;
    case '?':
      cag_option_print_error(&ctx, stderr);
      return 1;
    }
  }

  if (dist > CODE_BITS) {
    std::cerr << "cannot specify higher distance than available bits!" << std::endl;
    return 1;
  }

  std::srand(std::time(nullptr));
  std::vector<codeword> cs = { randomCodeword() };
  while (cs.size() < count) {
    codeword c = randomCodeword();
    uint8_t d = CODE_MAX;
    for (auto cc = cs.begin(); cc != cs.end(); cc++) {
      d = min(d, minDist(c, cc));
    }
    if (d >= dist) {
      cs.push_back(c);
    }
  }

  char buf[70];
  for (auto c : cs) {
    std::cout << std::itoa(*c, buf, 2) << std::endl;
  }

  return 0;
}
