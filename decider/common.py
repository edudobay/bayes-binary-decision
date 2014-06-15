import os
import numpy as np

datadir = os.path.expanduser('~/Decisao/data')
grafdir = os.path.expanduser('~/Decisao/tese/graficos')

agent_cls_tpl = [
    ('X',  'xxs'), ('Xi',  'xxi'), ('Xb',  'xxb'),
    ('XY', 'xys'), ('XYi', 'xyi'), ('XYb', 'xyb'),
    ('WS', 'wss'), ('WSi', 'wsi'), ('WSj', 'wsj'),
]
agent_tpl = dict(agent_cls_tpl)
agent_cls = dict((tpl,cls) for (cls,tpl) in agent_cls_tpl)

def Data(fname):
    return os.path.join(datadir, fname)

def DataFiles(fname, args, dtype=None):
    if dtype is None:
        return [Data(fname.format(arg)) for arg in args]
    elif dtype == 'dict':
        return [Data(fname.format(**argset)) for argset in args]
    elif dtype == 'list':
        return [Data(fname.format(*argset)) for argset in args]

def Graf(fname):
    return os.path.join(grafdir, fname)

def Items(d, keys):
    """Return a list of the values of a dict for the specified keys"""
    return [d[k] for k in keys]

def Agent(tpls):
    if isinstance(tpls, str):
        if tpls == 'all':
            return list(agent_cls.values())
        tpls = tpls.split()
    return [agent_cls[tpl] for tpl in tpls]

def AgentTpl(cls):
    if isinstance(cls, str):
        cls = cls.split()
    return [agent_tpl[c] for c in cls]

def expsmooth(seq, p):
    """Return an exponential smoothing of `seq`, with smoothing factor `p`."""
    smooth = np.zeros_like(seq)
    smooth[0] = seq[0]
    for i in range(1,len(seq)):
        smooth[i] = p * seq[i] + (1-p) * smooth[i-1]
    return smooth

def err_group(e, n, invert=False):
    """
     Given a time series of accumulated sums of errors, return a
     series of moving averages of `n` iterations.
    """
    eg = e[n:] - e[:len(e)-n]
    if invert: eg = n - eg
    return eg/n

def nicerange(start, end, spacing):
    """
     Return a grid with the specified spacing, starting and ending in multiples
     of the given spacing, and containing both specified endpoints.
    """
    xmin = np.floor(start / spacing) * spacing
    xticks = np.arange(xmin, end + spacing, spacing)
    xmax = xticks[-1]
    return xmin, xmax, xticks


