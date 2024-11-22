#include <cargs.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <sstream>

#define min(a, b) (a > b ? b : a)


typedef unsigned long long codeword;
uint8_t CODE_BITS = 43;
codeword CODE_MAX;


std::string tostring(codeword c) {
  std::ostringstream str;
  for (int i = CODE_BITS-1; i >= 0; i--) {
    codeword bit = ((codeword)1) << i;
    if (bit & c) {
      str << "1";
    } else {
      str << "0";
    }
  }
  return str.str();
}


codeword rotate(codeword c) {
  codeword inc = (c<<1) & CODE_MAX;
  codeword msb = (c >> (CODE_BITS-1)) & CODE_MAX;
  return inc | msb;
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
    c = c >> 8;
  }
  return count;
}
void initPop8() {
  for (unsigned int i=0; i<256; i++) {
    POP8[i] = popcount8(i);
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
  uint64_t a, b, c, d;
  a = rand() & 0xffff; b = rand() & 0xffff; c = rand() & 0xffff; d = rand() & 0xffff;
  codeword x = (a << 48) | (b << 32) | (c << 16) | d;
  x &= CODE_MAX;
  return x;
}



struct cag_option options[] = {
  { 'b', "b", "bits", "CODE_BITS", "number of bits per code" },
  { 'd', "d", "distance", "MIN_DIST", "minimum Hamming distance between each codeword" },
  { 'c', "c", "count", "COUNT", "size of the dictionary to generate" },
  { 'h', "h", "help", NULL, "show this help message" },
};



int main(int argc, char **argv) {
  initPop8();
  uint8_t dist = 20;
  uint8_t count = 32;
  cag_option_context ctx;
  cag_option_init(&ctx, options, CAG_ARRAY_SIZE(options), argc, argv);
  while(cag_option_fetch(&ctx)) {
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
  CODE_MAX = (((codeword)1) << CODE_BITS) - 1;

  if (dist > CODE_BITS) {
    std::cerr << "cannot specify higher distance than available bits!" << std::endl;
    return 1;
  }

  std::cout << "looking for " << std::to_string(count) 
    << " codes of length " << std::to_string(CODE_BITS) 
    << " and distance " << std::to_string(dist) << std::endl;

  std::srand(std::time(nullptr));

  // auto r = rotations(randomCodeword());
  // char buf[128];
  // for (auto c : r) {
  //   snprintf(buf, sizeof(buf), "%011llx", c);
  //   std::cout << tostring(c) << " " << std::to_string(popcount(c)) << std::endl;
  // }
  // snprintf(buf, sizeof(buf), "%llx", CODE_MAX);
  // std::cout << "max: " << buf << std::endl;
  // return 0;

  std::vector<codeword> cs = { randomCodeword() };
  while (cs.size() < count) {
    codeword c = randomCodeword();
    uint8_t d = CODE_MAX;
    std::cout << std::to_string(CODE_MAX) << std::endl;
    for (auto cc : cs) {
      d = min(d, minDist(c, cc));
    }
    std::cout << "[" << std::to_string(cs.size()) << "] attempting codeword " 
      << std::to_string(c) << " with distance " << std::to_string(d) << std::endl;
    if (d >= dist) {
      cs.push_back(c);
    }
  }

  for (auto c : cs) {
    std::cout << std::to_string(c) << std::endl;
  }

  return 0;
}
