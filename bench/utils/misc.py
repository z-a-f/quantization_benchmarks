import hashlib
import json
import os
import requests
import sys
import zipfile

from tqdm import tqdm

from . import PARENT_PATH

MODELS_PATH = os.path.join(PARENT_PATH, 'models')
MODELS_LINKS_PATH = os.path.join(MODELS_PATH, 'models.json')

DOWNLOAD_PATH = os.path.join(PARENT_PATH, 'downloads')

# def download_and_unzip(dataset_name, ):

def load_json(file_path):
  file_path = os.path.realpath(file_path)
  with open(file_path, 'r') as json_f:
    contents = json.load(json_f)
  return contents

def download_meta(meta, variant=None, download_path=None):
  if download_path is None:
    download_path = DOWNLOAD_PATH
  os.makedirs(download_path, exist_ok=True)
  for version, link in meta['links'].items():
    if download_file(link, download_path):
      return os.path.join(download_path, link.split('/')[-1])
  return None

def download_file(link, download_path=None, md5=None):
  if download_path is None:
    download_path = DOWNLOAD_PATH
  if not os.path.isdir(download_path):
    os.makedirs(download_path, exist_ok=True)
  file_name = link.split('/')[-1]
  file_path = os.path.join(download_path, file_name)
  if not os.path.isfile(file_path):
    print("Downloading the file...")
    with requests.get(link, stream=True) as req:
      total_length = req.headers.get('content-length')
      with open(file_path, 'wb') as f:
        if total_length is None:
          f.write(req.content)
        else:
          total_length = int(total_length) // 1000  # kB
          with tqdm(total=total_length, unit='kB') as pbar:
            for data in req.iter_content(chunk_size=4096):
              f.write(data)
              pbar.update(int(len(data) / 1000))
            pbar.n = total_length

    if 200 <= req.status_code < 300:
      print("DONE...")
      if md5 is not None:
        print("Checking hash...")
        with open(file_path, 'rb') as f:
          file_hash = hashlib.md5(f.read()).hexdigest()
          if file_hash != md5:
            print("Hash check failed")
            return False
      return True
    print("Download failed...")
    return False
  else:
    print("File exists, skipping download...")
    return True

def unzip_file(zip_file, path=None, skip_if_dir=True,
               cleanup_on_complete=False):
  if path is None:
    path = os.path.dirname(zip_file)
  if os.path.isdir(path) and skip_if_dir:
    return
  else:
    os.makedirs(path)
  print("Unzipping...")
  with zipfile.ZipFile(zip_file, 'r') as f:
    f.extractall(path)
  if cleanup_on_complete:
    os.remove(zip_file)
