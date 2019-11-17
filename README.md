Quantized Model Benchmarking
============================

Scripts to evaluate the run-time and the accuracy of the classification models under `torchvision`.
The dataset used for the evaluation is a 1000 image subset of ImageNet.
The images are resized to 256 pixels at the shortest edge, and center cropped to 224.


TODO
----

1. Documentation / Docstrings
2. Non-`vision` moodels
3. Larger datasets


Prerequisites
-------------

From `pip`:

- `numpy`
- `scipy`
- `json` - for loading/saving the data meta information and results
- `tqdm` - for progress meters
- `torch` - `nightly` version
- `torchvision` - `nightly` version

Structure
---------

- `bench` -> Small set of benchmarking routines
  - `data` -> Data loading routines
  - `models` -> Models loading routines
  - `metrics` -> Performance metrics and stuff
  - `utils` -> Some extra utilities


Current Results
---------------

### 2019.11.16 (CPU Only)

**Versions**

- `torchvision==v0.5.0.dev20191116`
- `torch==1.4.0.dev20191116`

**Invocation**

```
OMP_NUM_THREADS=1 MKL_NUM_THREADS=1 ./bench.py
```

**Results**

`TODO(z-a-f): Once the run is done, populate that!!!`
