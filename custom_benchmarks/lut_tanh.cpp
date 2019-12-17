// compile with
// $> c++ -std=c++14 -O3 ./lut.cpp

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

uint8_t get_from_lut(int32_t x, std::vector<uint8_t> lut) {
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

int main() {
  constexpr int RUNS = 100000;
  double time_span = 0;
  vector<uint8_t> lut;

  cout << "Number of runs: " << RUNS << endl;

  cout << "Creating a lookup table:" << endl;
  time_span = 0.0;
  for (int idx = 0; idx < RUNS; ++idx) {
    auto start = high_resolution_clock::now();
    lut = make_lut();
    auto stop = high_resolution_clock::now();
    time_span += duration_cast<duration<double, micro>>(stop - start).count();
  }
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "Fetching from the lookup table (sorted input):" << endl;
  time_span = 0.0;
  for (int idx = 0; idx < RUNS; ++idx) {
    int32_t x = int32_t(RUNS / 2.0);
    auto start = high_resolution_clock::now();
    auto y = get_from_lut(x, lut);
    auto stop = high_resolution_clock::now();
    time_span += duration_cast<duration<double, micro>>(stop - start).count();
  }
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "Fetching from the lookup table (random input):" << endl;
  time_span = 0.0;
  for (int idx = 0; idx < RUNS; ++idx) {
    int32_t x = rand() % RUNS / 2;
    auto start = high_resolution_clock::now();
    auto y = get_from_lut(x, lut);
    auto stop = high_resolution_clock::now();
    time_span += duration_cast<duration<double, micro>>(stop - start).count();
  }
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "std::tanh (sorted input, no quantization):" << endl;
  time_span = 0.0;
  for (int idx = 0; idx < RUNS; ++idx) {
    int32_t x = int32_t(RUNS / 2.0);
    auto start = high_resolution_clock::now();
    auto y = std::tanh(x);
    auto stop = high_resolution_clock::now();
    time_span += duration_cast<duration<double, micro>>(stop - start).count();
  }
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "std::tanh (random input, no quantization):" << endl;
  time_span = 0.0;
  for (int idx = 0; idx < RUNS; ++idx) {
    int32_t x = rand() % RUNS / 2;
    auto start = high_resolution_clock::now();
    auto y = std::tanh(x);
    auto stop = high_resolution_clock::now();
    time_span += duration_cast<duration<double, micro>>(stop - start).count();
  }
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "std::tanh (sorted input, with quantization):" << endl;
  time_span = 0.0;
  for (int idx = 0; idx < RUNS; ++idx) {
    int32_t x = int32_t(RUNS / 2.0);
    auto start = high_resolution_clock::now();
    auto dx = float(x - i_zp) * i_sc;
    auto y = std::tanh(dx);
    auto qy = int32_t(std::round(y * o_sc_inv + o_zp));
    qy = std::min<int32_t>(std::max<int32_t>(qy, qmin), qmax);
    auto stop = high_resolution_clock::now();
    time_span += duration_cast<duration<double, micro>>(stop - start).count();
  }
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;

  cout << "std::tanh (random input, with quantization):" << endl;
  time_span = 0.0;
  for (int idx = 0; idx < RUNS; ++idx) {
    int32_t x = rand() % RUNS / 2;
    auto start = high_resolution_clock::now();
    auto dx = float(x - i_zp) * i_sc;
    auto y = std::tanh(dx);
    auto qy = int32_t(std::round(y * o_sc_inv + o_zp));
    qy = std::min<int32_t>(std::max<int32_t>(qy, qmin), qmax);
    auto stop = high_resolution_clock::now();
    time_span += duration_cast<duration<double, micro>>(stop - start).count();
  }
  cout << "\tIt took me on average " << time_span / RUNS << " us/iter" << endl;
}
