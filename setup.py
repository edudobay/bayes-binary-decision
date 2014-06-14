from distutils.core import setup, Extension

ext_core = Extension('decider.core',
   include_dirs = ['src'],
   library_dirs = ['build/shared'],
   libraries = ['decider_dyn'],
   sources = ['pydecider.cpp']
)

setup(
   name = 'BayesDecider',
   version = '0.1',
   description = "Bayesian decision-making algorithm for predicting Markov chains, and related tools",
   author = 'Eduardo Dobay',
   author_email = 'edudobay@gmail.com',
   url = 'https://github.com/edudobay/decision',

   packages = ['decider'],
   ext_modules = [ext_core],

   requires = [
      'numpy'
   ],
)
