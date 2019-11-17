
import os

def _this_path(file=None):
  return os.path.dirname(__file__ if file is None else file)

PARENT_PATH = os.path.dirname(_this_path())

from .misc import *
