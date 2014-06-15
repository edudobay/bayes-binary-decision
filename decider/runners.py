import math
import string
import concurrent.futures
from .core import *
from .common import *

# Predefined machines
machines = {
   'm0': [0.3],
   'm1': [0.7, 0],
   'm2': [0.7, 0, 0.3, 1],
   'm1a': [0.7, 0.1],
   'm1b': [0.7, 0.3],
   'm1c': [0.3, 0.7],
   'm3a': [0.2, 0, 0.2, 0.6, 0.6, 0.8, 0.7, 0.3],
   'm2a': [0.3, 1, 0.7, 0],
}

# wrap decider.core.run() so that it can be pickled
def wrapped_run(**kwargs):
    return run(**kwargs)

def simulate(machine, aorder, aclasses, executor=None, **kwargs):
    """
     Run a batch of simulations using the specified parameters:
       machine  -- a key of the `machines` dict, or an array of probabilities
                   that will be passed directly to the Markov model, or a
                   parameter that will be passed to a custom `builder` kwarg
       aorder   -- memory size of the agent
       aclasses -- a list of agent class names (as defined in the C++ module)

     If the multiprocessing `executor` is given, all runs are parallelized in
     the executor's context.

     Possible extra parameters:
       fileNameFormat -- template for generating output filenames
       nRepetitions   -- how many times to run a simulation with the same
                         conditions (ignored if outputMode == 0)
     Any other arguments are passed directly to decider.core.run().
    """

    if 'builder' in kwargs:
        builder = kwargs.pop('builder')
        probs = builder(machine)
    elif isinstance(machine, list):
        probs = machine
    else:
        probs = machines[machine]
    morder = int(math.log2(len(probs)))

    outputMode = kwargs.get('outputMode', 0)

    default_file_name_format = { # outputMode
        0: '{machine}/o{aorder}{model}.{i:03}',
        1: '{machine}/o{aorder}{model}.rate',
        2: '{machine}/o{aorder}{model}.prob',
        3: '{machine}/o{aorder}{model}.ravg'
    }

    if outputMode not in [0, 1, 2, 3]:
        raise ValueError("invalid outputMode")

    fileNameFormat = kwargs.pop('fileNameFormat', default_file_name_format[outputMode])
    reps = kwargs.get('nRepetitions', 1000)

    params = {
        'agentOrder': aorder,
        'machineOrder': morder,
        'machineProbs': probs,
        'nIter': 500,
        'nRepetitions': reps,
    }
    params.update(kwargs)

    pool = []
    def _run(**params):
        if executor is not None:
            pool.append(executor.submit(wrapped_run, **params))
        else:
            run(**params)

    for cls in aclasses:
       params['agentClass'] = cls
       tpl = AgentTpl(cls)[0]
       if outputMode != 0:
          params['fileName'] = Data(fileNameFormat.format(model=tpl, machine=machine, aorder=aorder))
          _run(**params)
       else:
          params['nRepetitions'] = 1000
          for i in range(reps):
             params['fileName'] = Data(fileNameFormat.format(i=i, model=tpl, machine=machine, aorder=aorder))
             _run(**params)

    if executor is not None:
        return pool

def msimulate(*args, workers=8, **kwargs):
   """Calls `simulate` in a multiprocessing context, with a maximum of
   `workers` parallel processes, and waits for all processes to complete."""

   with concurrent.futures.ProcessPoolExecutor(max_workers=workers) as executor:
      futures = simulate(*args, executor=executor, **kwargs)
      for future in concurrent.futures.as_completed(futures):
         future.result()
      #print("simulate: completed")

class FormatDict(dict):
   """Subclass of `dict` that returns for missing keys a string formatting
   template: '{key}'."""

   def __missing__(self, key):
      return "{" + key + "}"

def lcsimulate(lcs, *args, **kwargs):
   """Repeat (via msimulate) the simulation specified by all the arguments for
   the given sequence of pairs of learning modulation coefficients."""

   fileNameFormat = kwargs.get('fileNameFormat', '{machine}/o{aorder}{model}_L{LC}.rate')
   if 'outputMode' not in kwargs:
      kwargs['outputMode'] = 1

   formatter = string.Formatter()

   for (c0, c1) in lcs:
      lcstr = '{0:03}_{1:03}'.format(int(c0 * 100), int(c1 * 100))
      kwargs['agentLC'] = (c0, c1)
      kwargs['fileNameFormat'] = formatter.vformat(fileNameFormat, (), FormatDict(LC=lcstr))
      msimulate(*args, **kwargs)

