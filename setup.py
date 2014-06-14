from distutils.core import setup, Extension

module1 = Extension('decider',
   include_dirs = ['src'],
   library_dirs = ['build/shared'],
   libraries = ['decider_dyn'],
   sources = ['pydecider.cpp']
)

setup(
   name = 'PyDecider',
   version = '0.1',
   description = 'DeciderModule1',
   ext_modules = [module1]
)
