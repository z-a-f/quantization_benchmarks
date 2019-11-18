import json
import numpy as np

with open('results.json', 'r') as f:
  results = json.load(f)

fp = results['fp']
q = results['quantized']

template = r"""| Name              | Quantized Time <br/>Relative to FP | FP Top-1 Accuracy | Quantized Top-1 Accuracy |"""
columns = template.split('|')
lengths = []
for idx in range(len(columns)):
  # if len(columns[idx]) == 0:
  #   continue
  lengths.append(len(columns[idx]))
lines = ['-'*l for l in lengths]
line = '|'.join(lines)
body = "|{{name:<{}}}|{{qf_time:^{}.2f}}|{{f_acc:^{}.2%}}|{{q_acc:^{}.2%}}|".format(*lengths[1:-1])

print(template)
print(line)
for name, q_perf in q.items():
  perf = {}
  f_perf = fp[name]
  perf['name'] = name
  perf['qf_time'] = (q_perf['train']['time'] / f_perf['train']['time'] +
                     q_perf['test']['time'] / f_perf['test']['time']) / 2.0
  perf['f_acc'] = (f_perf['train']['accuracy'] + f_perf['test']['accuracy']) / 2.0
  perf['q_acc'] = (q_perf['train']['accuracy'] + q_perf['test']['accuracy']) / 2.0

  print(body.format(**perf))



# TODO(z-a-f): Finish this method of generating markdown...

# """
# List of variables:

# - "name" -- model name as saved in the "results.json"
# - "xxx_yyy_zzz" -- see description below

# Given that

# - "xxx" could be
#   - "f" for absolute results for fp models
#   - "q" for absolute results for quantized models
#   - "fq" for relative results (fp / quantized)
#   - "qf" for relative results (quantized / fp)
# - "yyy" could be
#   - "train", results acquired using training set
#   - "test", results acquired using test set
#   - "avg", average results weighted by the number of train and test examples
# - "zzz" is the metric. Currently, could be
#   - "time" -- runtime
#   - "accuracy" -- classification accuracy
# """

# template = r"""{name} | {qf_avg_time} | {f_avg_accuracy:.2%} | {q_avg_accuracy:.2%}"""

# heading = {
#   'name': 'Name',
#   'time': 'time',
#   'accuracy': 'accuracy',
#   'train': 'train',
#   'test': 'test',
#   'avg': 'average',
#   'f': 'FP',
#   'q': 'Quantized',
#   'fq': 'FP / Quantized',
#   'qf': 'Quantized / FP'
# }

# json_var_mapping = {
#   'f': 'fp',
#   'q': 'quantized',
#   'time': None
# }

# def get_columns(template):
#   columns = [col.strip() for col in template.split('|')]
#   res = {}
#   for idx, col in enumerate(columns):
#     col = columns[idx].strip('{').strip('}')
#     col = col.split(':')[0]
#     key = columns[idx]
#     columns[idx] = columns[idx].split('_')

#   return columns

# if __name__ == '__main__':
#   print(get_columns(template))





