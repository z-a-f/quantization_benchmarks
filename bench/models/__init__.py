
import os

from ..utils import PARENT_PATH
from ..utils import load_json

MODELS_PATH = os.path.join(PARENT_PATH, 'models')
MODELS_LINKS_PATH = os.path.join(MODELS_PATH, 'models.json')

def load_model_meta(model_name, links_file=None):
  contents = load_json(MODELS_LINKS_PATH if links_file is None else links_file)
  return contents.get(model_name, None)

def get_all_model_pairs(links_file=None):
  if links_file is None:
    links_file = MODELS_LINKS_PATH
  models = load_json(links_file)
  q_models = models.get('quantization')
  f_models = models.get('fp')

  valid_pairs = []
  for name, meta in q_models.items():
    if f_models.get(name, None) is None:
      continue
    valid_pairs.append((name, meta, f_models[name]))
  return valid_pairs

def get_all_model_imports(links_file=None):
  valid_model_pair_links = get_all_model_pairs(links_file)
  models = []
  for name, q_meta, f_meta in valid_model_pair_links:
    for variant, import_link in q_meta['imports'].items():
      f_variant = variant
      if variant.endswith('qnnpack') or variant.endswith('fbgemm'):
        f_variant = '_'.join(f_variant.split('_')[:-1])
      if q_meta['links'][variant] is None:
        continue
      if f_meta['links'][f_variant] is None:
        continue
      models.append((name, import_link, f_meta['imports'][f_variant]))
  return models
