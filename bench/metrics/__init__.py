
import numpy as np
import time
import torch

class Performance:
  def __init__(self, model, loader, topk=(1,)):
    self.model = model
    self.loader = loader
    self.topk = topk
    self.time = None
    self.accuracy = None
    self.reset()

  def inference(self):
    _time = 0.0
    _count = 0
    _accuracy = np.array([0.0 for _ in range(len(self.topk))])
    with torch.no_grad():
      for idx, (img, lbl) in enumerate(self.loader):
        print('.', end='')
        batch_time = time.time()
        batch_result = self.model(img)
        batch_time = time.time() - batch_time
        acc = accuracy(batch_result, lbl, topk=self.topk)
        for idx in range(len(self.topk)):
          _accuracy[idx] += acc[idx].item() * img.shape[0]
        _time += batch_time
        _count += img.shape[0]

        self.time = _time
        self.accuracy = _accuracy / _count
    print()
    return self.time, self.accuracy

  def reset(self):
    time = self.time
    accuracy = self.accuracy
    self.time = 0.0
    self.accuracy = np.array([0.0 for _ in range(len(self.topk))])
    return time, accuracy


def accuracy(output, target, topk=(1,)):
  """Computes the accuracy over the k top predictions for the specified values of k"""
  maxk = max(topk)
  batch_size = target.size(0)

  _, pred = output.topk(maxk, 1, True, True)
  pred = pred.t()
  correct = pred.eq(target.view(1, -1).expand_as(pred))

  res = []
  for k in topk:
    correct_k = correct[:k].view(-1).float().sum(0, keepdim=True)
    res.append(correct_k.mul_(1.0 / batch_size))
  return res
