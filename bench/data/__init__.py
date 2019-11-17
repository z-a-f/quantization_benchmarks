
import os

from ..utils import PARENT_PATH
from ..utils import download_file
from ..utils import load_json

DATA_PATH = os.path.join(PARENT_PATH, 'data')
DATA_LINKS_PATH = os.path.join(DATA_PATH, 'data.json')

def get_dataset_list(links_file=None):
  contents = load_json(DATA_LINKS_PATH if links_file is None else links_file)
  return list(contents.keys())

def load_dataset_meta(dataset_name, links_file=None):
  contents = load_json(DATA_LINKS_PATH if links_file is None else links_file)
  return contents.get(dataset_name, None)

def get_data(dataset_name):
  pass
