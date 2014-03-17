#!/usr/bin/python

import numpy as np
from scipy.special import erf, beta, betainc
from scipy.misc import factorial

stepf = lambda x: (np.sign(x) + 1)/2
betareg = betainc

def q_estimators(mu, j):
   #j = np.arange(mu+1)
   x = j / mu
   #sigma = np.sqrt(j*(mu-j)/mu**3)
   sigma = np.sqrt(1/mu/4)
   return ([np.array([0, 0.5, 0.5, 1]), x, x],
   [
      np.array([0, 0, 1, 1]),
      #stepf(2*j - mu),
      betareg(mu-j+1, j+1, 0.5),
      (1 + erf((j - mu/2) / (mu*sigma*np.sqrt(2))))/2
   ])

def distr_p(mu, j, p):
   norm = factorial(j) * factorial(mu - j) / factorial (mu + 1)
   return p**j * (1-p)**(mu-j) / norm

# ----------------------------------------------------------------------------
# Matplotlib setup

import matplotlib as mpl
rc = {
   'font.family': 'Linux Libertine',
   'font.serif': [],
   'font.sans-serif': ['Open Sans'],
   'pgf.preamble': [
      r"\usepackage{unicode-math}",  # unicode math setup
      r"\setmathfont{xits-math.otf}",
   ],
   'axes.linewidth': 0.6,
   'lines.linewidth': 1.0,
}
mpl.rcParams.update(rc)

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pgf import FigureCanvasPgf
mpl.backend_bases.register_backend('pdf', FigureCanvasPgf)

# ----------------------------------------------------------------------------
# Gráficos teóricos

def plot_q_estimators(mu, fname):
   mu = int(mu)
   j = np.linspace(0,mu,200)
   xs, ys = q_estimators(mu, j)

   plt.figure(figsize=(4.5,2.5))
   plt.plot(xs[0], ys[0], '-', c='#44dd44', label="$q^A$")
   plt.plot(xs[1], ys[1], '-', c='r',       label="$q^B$")
   plt.plot(xs[2], ys[2], '-', c='b',       label="$q^C$")

   leg = plt.legend(loc='lower right', fontsize='small',
         fancybox=True, labelspacing=0.25,
         borderaxespad=1.0,
         title=r'\textbf{{{{\itshape µ}} = {0}}}'.format(mu)
         )
   leg_frame = leg.get_frame()
   leg_frame.set_linewidth(0.5)
   leg_frame.set_edgecolor('#999999')
   leg_frame.set_facecolor('#ffffcc')
   plt.tight_layout(pad=.5, rect=[0,0.08,1,1]) # extra space for x label
   plt.ylim(-0.02, 1.02) # extra y space to avoid conflict between plots and axes
   plt.xlabel(r"$j/\mu$")
   plt.savefig(fname)

def _plot_p_distr(mu, js, fname):
   plot_p_distr(mu, js.split(','), fname)

def plot_p_distr(mu, js, fname):
   mu = int(mu)
   js = [int(j) for j in js]
   p = np.linspace(0, 1, 100)
   #data = [(distr_p(mu, int(j), p), int(j)) for j in np.linspace(0, mu-1, 5)]
   data = [(distr_p(mu, j, p), j) for j in js]

   plt.figure(figsize=(4.5,2.5))
   for y, j in data:
      plt.plot(p, y, '-', label="$j = {0}$".format(j))

   leg = plt.legend(loc='best', fontsize='small',
         fancybox=True, labelspacing=0.25,
         borderaxespad=1.0,
         title=r'\textbf{{{{\itshape µ}} = {0}}}'.format(mu)
         )
   leg_frame = leg.get_frame()
   leg_frame.set_linewidth(0.5)
   leg_frame.set_edgecolor('#999999')
   leg_frame.set_facecolor('#ffffcc')

   plt.tight_layout(pad=.5)#, rect=[0,0.08,1,1]) # extra space for x label
   #plt.ylim(-0.02, 1.02) # extra y space to avoid conflict between plots and axes
   #plt.xlabel(r"$p$")
   plt.savefig(fname)


# ----------------------------------------------------------------------------
# Gráficos de simulações (séries temporais)

def _plotsim_cumerrorrate(*args):
   fn_input, args = args_till_semicolon(args)
   labels, args = args_till_semicolon(args)
   try:
      size, fn_output = args[:2]
      args = args[2:]
      size = int(size)
   except IndexError:
      raise ArgumentError
   except ValueError:
      raise ArgumentError

   plotsim_cumerror(fn_input, labels, size, fn_output, rate=True, **extra_args(args))

def _plotsim_cumerror(*args):
   fn_input, args = args_till_semicolon(args)
   labels, args = args_till_semicolon(args)
   try:
      size, fn_output = args[:2]
      args = args[2:]
      size = int(size)
   except IndexError:
      raise ArgumentError
   except ValueError:
      raise ArgumentError

   plotsim_cumerror(fn_input, labels, size, fn_output, **extra_args(args))

def plotsim_cumerror(fn_input, labels, size, fn_output, rate=False, **extra):
   x = np.arange(1,size+1)
   ys = []
   for fn in fn_input:
      data = np.loadtxt(fn)[:size]
      if len(data) < size:
         size = len(data)
      cumerror = data[:,2]
      #cumerror = x-cumright
      if rate:
         ys.append(cumerror / x)
      else:
         ys.append(cumerror)

   plt.figure(figsize=(4.5,2.5))
   for i, cumerror in enumerate(ys):
      if labels:
         plt.plot(x, cumerror, '-', label=labels[i])
      else:
         plt.plot(x, cumerror, '-')

   if labels and extra.get('leg', True):
      leg = plt.legend(loc='best', fontsize='small',
            fancybox=True, labelspacing=0.25,
            borderaxespad=1.0,
            #title=r'\textbf{{{{\itshape µ}} = {0}}}'.format(mu)
            )
      leg_frame = leg.get_frame()
      leg_frame.set_linewidth(0.5)
      leg_frame.set_edgecolor('#999999')
      leg_frame.set_facecolor('#ffffcc')

   plt.tight_layout(pad=.5, rect=[0.06,0.08,1,1]) # extra space for x label
   plt.xlabel(r'\itshape tempo')
   if rate:
      plt.ylabel(r'\itshape taxa de erros')
   else:
      plt.ylabel(r'\itshape número acumulado de erros')
   plt.savefig(fn_output)

# ----------------------------------------------------------------------------

def plot_errvscl(fn_input, xlabel, ylabel, fn_output):
   raise NotImplemented
   plt.figure(figsize=(4.5,2.5))
   plt.plot(x, cumerror, '-')

   plt.tight_layout(pad=.5, rect=[0.06,0.08,1,1]) # extra space for x label
   plt.xlabel(r'\itshape ' + xlabel)
   plt.ylabel(r'\itshape ' + ylabel)
   plt.savefig(fn_output)


# ----------------------------------------------------------------------------
# Início

class ArgumentError(RuntimeError):
   pass

def args_till_semicolon(args, fail=True):
   if args is None and fail:
      raise ArgumentError
   try:
      j = args.index(';')
      return args[:j], args[j+1:]
   except ValueError:
      if fail:
         raise ArgumentError
      return args, None

# only bool args for now
def extra_args(args):
   parsed_args = {}
   for arg in args:
      if arg.startswith('-'):
         parsed_args[arg[1:]] = False
      elif arg.startswith('+'):
         parsed_args[arg[1:]] = True
   return parsed_args

if __name__ == "__main__":
   import sys

   dispatchers = {
      'p': _plot_p_distr,
      'q': plot_q_estimators,
      'simcume': _plotsim_cumerror,
      'simcumer': _plotsim_cumerrorrate,
      'errvscl': plot_errvscl,
   }

   name = sys.argv[1]
   try:
      dispatchers[name](*sys.argv[2:])
   except ArgumentError:
      print("argument error for procedure '{0}'".format(name))
      sys.exit(1)

