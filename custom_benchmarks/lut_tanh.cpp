// compile with
// $> c++ -std=c++14 -O3 ./lut_tanh.cpp

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;
using namespace std::chrono;

constexpr int32_t input_min = -128;
constexpr int32_t input_max = 127;
constexpr uint8_t qmin = 0;
constexpr uint8_t qmax = 255;
constexpr int8_t i_zp = 0;
constexpr float i_sc = 8.0f / 256.0f;
constexpr uint8_t o_zp = 128;
constexpr float o_sc_inv = 256.0f / 2.0;

template <typename T>
void print_vector(std::vector<T> x) {
  std::cout << "{ ";
  for (const auto& xx : x) {
    std::cout << double(xx) << " ";
  }
  std::cout << "}" << std::endl;
}

template <typename T>
T clamp(float x, T min, T max) {
  x = std::min<float>(x, max);
  x = std::max<float>(x, min);
  return T(x);
}

// Bench the UINT8 quantization:
// Dequantized Range Assumptions:
//   x: [-4.0, 4.0]  Quantized Input range: [-128, 127]
//   y: [-1.0, 1.0]
//   q: [0, 255]
// Q paraus assumptions (tanh input):
//   scale: 2.0 / 255
//   zp: 128
// Q paraus assumptions (tanh output):
//   scale: 2.0 / 255
//   zp: 128

// Creates a LUT
vector<uint8_t> make_lut() {
  vector<uint8_t> result;
  result.reserve(256);

  const float scaled_min = (float)(int32_t)qmin;
  const float scaled_max = (float)(int32_t)qmax;

  for (int32_t idx = input_min; idx < input_max; ++idx) {
    const float x = i_sc * (float)(idx - (int32_t)(uint8_t)i_zp);
    float scaled_tanh_x = clamp(o_sc_inv * std::tanh(x) + o_zp, qmin, qmax);
    if (scaled_tanh_x < scaled_min) {
      scaled_tanh_x = scaled_min;
    }
    if (scaled_tanh_x > scaled_max) {
      scaled_tanh_x = scaled_max;
    }
    result.push_back((uint8_t)lrintf(scaled_tanh_x));
  }
  return result;
}

uint8_t get_from_lut(int32_t x, const std::vector<uint8_t>& lut) {
  x = x - input_min;
  const int32_t idx_max = 255;
  if (x < 0) {
    return lut[0];
  }
  if (x > idx_max) {
    return lut[idx_max];
  }
  return lut[x];
}

// Creating a LUT
void run_make_lut(int runs) {
  for (int idx = 0; idx < runs; ++idx) {
    volatile vector<uint8_t> lut = make_lut();
  }
}

// Getting from LUT (sorted)
template <bool SORTED>
__attribute__((noinline)) void run_access_lut(int runs, vector<uint8_t> lut) {
  int start = -(runs >> 1);
  int stop = -start - 1;
  for (int idx = start; idx < stop; ++idx) {
    int32_t x;
    if (SORTED) {
      x = idx;
    } else {
      x = rand() % runs + start;
    }
    volatile auto y = get_from_lut(x, lut);
  }
}

// cmath impl
template <bool SORTED, bool QUANT>
__attribute__((noinline)) void run_cmath(int runs) {
  int start = -(runs >> 1);
  int stop = -start - 1;
  for (int idx = start; idx < stop; ++idx) {
    volatile int32_t x;
    if (SORTED) {
      x = idx;
    } else {
      x = rand() % runs + start;
    }
    if (QUANT) {
      auto dx = float(x - i_zp) * i_sc;
      auto y = std::tanh(dx);
      auto qy = int32_t(std::round(y * o_sc_inv + o_zp));
      volatile auto y_out = std::min<int32_t>(std::max<int32_t>(qy, qmin), qmax);
    } else {
      volatile auto y = std::tanh(x);
    }
  }
}

template <typename TIME_UNIT=micro>
double count(high_resolution_clock::time_point t0, high_resolution_clock::time_point t1) {
  return duration_cast<duration<double, TIME_UNIT>>(t1 - t0).count();
}

int main() {
  constexpr int RUNS = 1000000;
  high_resolution_clock::time_point start;
  high_resolution_clock::time_point stop;
  double time_span = 0;

  cout << "Number of runs: " << RUNS << endl;

  cout << "Creating a lookup table:" << endl;
  start = high_resolution_clock::now();
  asm volatile("run_make_lut0:\n\t");
  run_make_lut(RUNS);
  stop = high_resolution_clock::now();
  time_span = count(start, stop);
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "Fetching from the lookup table (sorted input):" << endl;
  vector<uint8_t> lut = make_lut();
  start = high_resolution_clock::now();
  asm volatile("run_access_lut0:\n\t");
  run_access_lut<true>(RUNS, lut);
  stop = high_resolution_clock::now();
  time_span = count(start, stop);
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "Fetching from the lookup table (random input):" << endl;
  start = high_resolution_clock::now();
  asm volatile("run_access_lut1:\n\t");
  run_access_lut<false>(RUNS, lut);
  stop = high_resolution_clock::now();
  time_span = count(start, stop);
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "std::tanh (sorted input, no quantization):" << endl;
  start = high_resolution_clock::now();
  asm volatile("run_cmath0:\n\t");
  run_cmath<true, false>(RUNS);
  stop = high_resolution_clock::now();
  time_span = count(start, stop);
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "std::tanh (random input, no quantization):" << endl;
  start = high_resolution_clock::now();
  asm volatile("run_cmath1:\n\t");
  run_cmath<false, false>(RUNS);
  stop = high_resolution_clock::now();
  time_span = count(start, stop);
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "std::tanh (sorted input, with quantization):" << endl;
  start = high_resolution_clock::now();
  asm volatile("run_cmath2:\n\t");
  run_cmath<true, true>(RUNS);
  stop = high_resolution_clock::now();
  time_span = count(start, stop);
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "std::tanh (random input, with quantization):" << endl;
  start = high_resolution_clock::now();
  asm volatile("run_cmath3:\n\t");
  run_cmath<false, true>(RUNS);
  stop = high_resolution_clock::now();
  time_span = count(start, stop);
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;
}
