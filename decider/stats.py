import re
import glob
import numpy as np

def lcloaddata(pattern, units=1e-2):
   def pattern_regex(pattern):
      regex = ""
      placeholders = 0
      while True:
         index = pattern.find("*")
         if index == -1:
            regex += re.escape(pattern)
            break
         regex += re.escape(pattern[:index]) + "([0-9]+)"
         placeholders += 1
         pattern = pattern[index+1:]

      if placeholders != 2:
         raise ValueError("invalid pattern: {0!r}".format(pattern))
      return re.compile(regex)

   regex = pattern_regex(pattern)
   dt = np.dtype([ ('c1', np.float64), ('c2', np.float64), ('mean', np.float64), ('std', np.float64) ])

   def gen():
      for fname in sorted(glob.glob(pattern)):
         m = regex.match(fname)
         c1, c2 = map(lambda x: int(x) * units, m.groups())
         v = np.loadtxt(fname)
         yield (c1, c2, np.average(v), np.std(v))

   return np.fromiter(gen(), dt)

def lcsummary(name):
   v = lcloaddata('{0}_L*_*.rate'.format(name))
   np.savetxt('{0}_Lsum.rate'.format(name), v, fmt='%g', delimiter='\t')

