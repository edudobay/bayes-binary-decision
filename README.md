decision
========

Simulation of decision-making bayesian agents; analysis (mainly graphs) of results.

The simulations themselves are done in a C++ program, which is wrapped inside a
Python extension module to ease running.

Build instructions
------------------

**Requirements:**

* g++
* SCons for building the C++ library
* Python 3 (I use 3.4 but I'm not sure what the bare minimum is; I think it is 3.2)
* numpy for some data manipulation and analysis

**Steps:**

* Run `scons` to build the C++ library.

* Run `python setup.py install --user` to install the Python utilities (runners
  and some data manipulation and analysis tools) at your local Python
  repository (no need for `sudo`)
  - You can also use `pip install --user .` to install via *pip* — you can later
    uninstall using `pip uninstall BayesDecider`

*Et voilà!* You can use it in any Python console; look for the `decider.runners` module.

```python
from decider.runners import *
simulate(...)
```

