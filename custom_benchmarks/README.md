
# Custom Benchmarks

## TanH implementation -- `cmath` vs custom LUT

```console
$> c++ -std=c++14 -O3 ./lut_tanh.cpp && ./a.out
Number of runs: 1000000
Creating a lookup table:
  It took me on average 3.22996 us/iter
Fetching from the lookup table (sorted input):
  It took me on average 0.0016141 us/iter
Fetching from the lookup table (random input):
  It took me on average 0.0143325 us/iter
std::tanh (sorted input, no quantization):
  It took me on average 0.00239779 us/iter
std::tanh (random input, no quantization):
  It took me on average 0.00662327 us/iter
std::tanh (sorted input, with quantization):
  It took me on average 0.00585652 us/iter
std::tanh (random input, with quantization):
  It took me on average 0.0204291 us/iter
```

**Random inputs + re-quantization (in `std::tanh`):**

`std::tanh` is ~10-40% slower ...
