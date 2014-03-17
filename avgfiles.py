#!/usr/bin/python

import numpy as np

def average_files(input_format, input_range, output):
   print('starting batch to', output)
   total = None
   count = 0

   for i in input_range:
      fn = input_format.format(i)
      datum = np.loadtxt(fn)
      if total is None:
         total = datum
      else:
         total += datum
      count += 1

   avg = total / count
   np.savetxt(output, avg)

datadir = '/home/eduardo/Decisao/data'

for tpl in 'wss wsi wsj'.split():
   average_files(datadir + '/m1b/' + tpl + '.{0:03}',
         range(1000), datadir + '/m1b/' + tpl + '.avg')

