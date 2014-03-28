from ctypes import *
import os

__all__ = ['RunInfo', 'set_datadir', 'run', 'create_runinfo']

datadir = os.path.expanduser('~/Decisao/data')
if os.name == 'nt':
   _lib = CDLL('./build/decider.dll')
else:
   _lib = CDLL('./build/libdecider.so')

def set_datadir(path):
   global datadir
   datadir = path

class RunInfo(Structure):
   _fields_ = [
      ('agentClass', c_char_p),
      ('agentOrder', c_int),
      ('agentLC', POINTER(c_double)),
      ('agentSeed', c_uint32),
      ('machineOrder', c_int),
      ('machineProbs', POINTER(c_double)),
      ('machineSeed', c_uint32),
      ('nRuns', c_int),
      ('fileName', c_char_p),
      ('printScore', c_int),
      ('outputMode', c_int),
      ('nRepetitions', c_int),
      ('nScore', c_int),
      ('shiftProbsStep', c_int),
   ]

def make_array(args):
   return (c_double * len(args))(*args)

def asciiize(s):
   return s.encode('ascii') if isinstance(s, str) \
     else s

def Data(fname):
   return (datadir + '/' + fname).encode('ascii')

def create_runinfo(**kwargs):
   ri = RunInfo(
      agentClass=asciiize(kwargs['agentClass']),
      agentOrder=kwargs['agentOrder'],
      agentLC=make_array(kwargs.get('agentLC', (1.0, 1.0))),
      agentSeed=kwargs.get('agentSeed', 0),
      machineOrder=kwargs['machineOrder'],
      machineProbs=make_array(kwargs['machineProbs']),
      machineSeed=kwargs.get('machineSeed', 0),
      nRuns=kwargs['nRuns'],
      fileName=Data(kwargs['fileName']),
      printScore=int(bool(kwargs.get('printScore', False))),
      outputMode=int(kwargs.get('outputMode', 0)),
      nRepetitions=int(kwargs.get('nRepetitions', 1)),
      nScore=int(kwargs.get('nScore', 0)),
      shiftProbsStep=int(kwargs.get('shiftProbsStep', 0)),
   )
   return ri

def run(runinfo=None, **kwargs):
   if runinfo is None:
      runinfo = create_runinfo(**kwargs)

   _lib.runModel(byref(runinfo))

