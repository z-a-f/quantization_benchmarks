
# Custom Benchmarks

## TanH implementation -- `cmath` vs custom LUT

```console
$> c++ -std=c++14 -O3 ./lut_tanh.cpp
$> a.out

Number of runs: 100000
Creating a lookup table:
  It took me on average 3.57787 us/iter
Fetching from the lookup table (sorted input):
  It took me on average 0.197345 us/iter
Fetching from the lookup table (random input):
  It took me on average 0.193709 us/iter
std::tanh (sorted input, no quantization):
  It took me on average 0.0411573 us/iter
std::tanh (random input, no quantization):
  It took me on average 0.040176 us/iter
std::tanh (sorted input, with quantization):
  It took me on average 0.0384812 us/iter
std::tanh (random input, with quantization):
  It took me on average 0.039479 us/iter
```

Don't read too much into the difference between quantized and non-quantized runs -- the runtime is pretty much the same
.
