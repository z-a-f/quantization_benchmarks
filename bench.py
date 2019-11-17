#!/usr/bin/env python

"""

Running bench:

```
OMP_NUM_THREADS=1 MKL_NUM_THREADS=1 ./bench.py
```

"""

import torch
torch.set_num_threads(1)

## === Data prep

from bench.utils import download_meta, unzip_file
from bench.data import load_dataset_meta, DATA_PATH

dataset = 'imagenet_1k_tiny'

dataset_meta = load_dataset_meta(dataset)
dataset_path = download_meta(dataset_meta)
if dataset_path is not None:
  unzip_file(dataset_path, cleanup_on_complete=False)
dataset_path = '.'.join(dataset_path.split('.')[:-1])

## === Data Loaders
from bench.data.utils import prepare_data_loaders
train_loader, test_loader = prepare_data_loaders(dataset_path, 256)
test_loader.dataset.transform.transforms[0].size = 310
test_loader.dataset.transform.transforms[1].size = 300
train_loader.dataset.transform = test_loader.dataset.transform


# print(train_loader.dataset.transform.transforms)
# print(test_loader.dataset.transform)
# STOP
# === Models
from collections import defaultdict
import json

from torchvision import models as f_models
from torchvision.models import quantization as q_models
from bench.metrics import Performance
from bench.models import get_all_model_imports

SKIP_MODELS = (
  'shufflenetv2',  # There might be a bug in the model (pytorch/vision#1590)
)

ONLY_MODELS = (
  'googlenet',
)

results = {
  'fp': defaultdict(lambda: defaultdict(dict)),
  'quantized': defaultdict(lambda: defaultdict(dict))
}

for name, q_import, f_import in get_all_model_imports():
  if name in SKIP_MODELS:
    continue
  # if name not in ONLY_MODELS:
  #   continue

  f_model = getattr(f_models, f_import)(pretrained=True, progress=True)
  q_model = getattr(q_models, q_import)(pretrained=True, progress=True,
                                        quantize=True)
  f_model.eval()
  q_model.eval()

  for mode in ('train', 'test'):
    if mode == 'train':
      loader = train_loader
    else:
      loader = test_loader
    print("Checking {} using {} data".format(name, mode))

    f_perf = Performance(f_model, loader)
    time, accuracy = f_perf.inference()
    results['fp'][f_import][mode]['time'] = time
    results['fp'][f_import][mode]['accuracy'] = accuracy[0]

    with open('results_f_'+mode+'_'+f_import+'.json', 'w') as f:
      json.dump(results['fp'][f_import][mode], f, indent=4)

    q_perf = Performance(q_model, loader)
    time, accuracy = q_perf.inference()
    results['quantized'][q_import][mode]['time'] = time
    results['quantized'][q_import][mode]['accuracy'] = accuracy[0]

    with open('results_q_'+mode+'_'+q_import+'.json', 'w') as f:
      json.dump(results['fp'][f_import][mode], f, indent=4)

print(results)
# === Dump the data
import json
json_dump_file = 'results.json'

with open(json_dump_file, 'w') as f:
  json.dump(results, f, indent=4)
