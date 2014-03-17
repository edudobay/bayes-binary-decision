#!/usr/bin/python

from decider import *
import math
#import sys

machines = {
   'm0': [0.3],
   'm1': [0.7, 0.1],
   'm1b': [0.7, 0],
}

for cls, tpl in \
 [
   ('X',  'xxs'), ('Xi',  'xxi'), ('Xb',  'xxb'),
   ('XY', 'xys'), ('XYi', 'xyi'), ('XYb', 'xyb'),
   ('WS', 'wss'), ('WSi', 'wsi'), ('WSj', 'wsj'),
 ]:

   aorder = 1
   machine = 'm1'

   probs = machines[machine]
   morder = int(math.log2(len(probs)))

   for i in range(1000):
      run(
         agentClass=cls,
         fileName='{machine}/o{o}{model}.{i:03}'.format(i=i, model=tpl, machine=machine, o=aorder),
         agentOrder=aorder,
         #machineSeed=12504,
         machineOrder=morder,
         machineProbs=probs,
         nRuns=500
      )

