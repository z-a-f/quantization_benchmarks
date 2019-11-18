Quantized Model Benchmarking
============================

Scripts to evaluate the run-time and the accuracy of the classification models under `torchvision`.
The dataset used for the evaluation is a [1000 image subset of ImageNet](https://s3.amazonaws.com/pytorch-tutorial-assets/imagenet_1k.zip).
The images are resized to 256 pixels at the shortest edge, and center cropped to 224.


TODO
----

1. Documentation / Docstrings
2. Add ability to save data meta information in the results
3. Non-`vision` moodels
4. Larger datasets


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

| Name              | Quantized Time <br/>Relative to FP | FP Top-1 Accuracy | Quantized Top-1 Accuracy |
|-------------------|------------------------------------|-------------------|--------------------------|
|googlenet          |                0.37                |      73.45%       |          73.25%          |
|inception_v3       |                0.42                |      77.80%       |          77.95%          |
|mobilenet_v2       |                0.38                |      72.40%       |          73.15%          |
|resnet18           |                0.62                |      70.00%       |          69.45%          |
|resnet50           |                0.37                |      76.95%       |          76.60%          |
|resnext101_32x8d   |                0.47                |      79.60%       |          79.85%          |
