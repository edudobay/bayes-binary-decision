import numpy as np

def score(x1, x2, relative=True, start=0, end=None):
    if len(x1) != len(x2): raise ValueError
    if end is None: end = len(x1)
    _score = sum(1 for i in range(start, end) if x1[i] == x2[i])
    if relative: _score = _score / (end - start)
    return _score

def _correlation(x1, x2):
    if len(x1) != len(x2): raise ValueError
    return sum((1 if x1[i] == x2[i] else -1) for i in range(len(x1))) / len(x1)

def correlation(x1, x2, maxlevel, start=0, end=None):
    """Calculates correlation of x1 and x2 looking at x2 relative to x1's past,
    up to `maxlevel` steps."""
    if len(x1) != len(x2): raise ValueError
    if maxlevel >= len(x1): raise ValueError
    if end is None: end = len(x1)
    return np.array([_correlation(x1[start:end - level], x2[start + level:])
        for level in range(0, maxlevel + 1)])

def symcorrelation(x1, x2, maxlevel, start=0, end=None):
    """Calculates correlation of x1 and x2 looking at x2 relative to x1's past
    (positive indices) or future (negative indices), up to `maxlevel` steps.
    Results should be understood as having indices from `-maxlevel` to
    `maxlevel`."""
    return np.concatenate((correlation(x2, x1, maxlevel, start, end)[:0:-1],
                           correlation(x1, x2, maxlevel, start, end)))

def autocorrelation(x1, maxlevel, start=0, end=None):
    return correlation(x1, x1, maxlevel, start, end)

def symautocorrelation(x1, maxlevel, start=0, end=None):
    pos = correlation(x1, x1, maxlevel, start, end)  # positive part
    return np.concatenate((pos[:0:-1], pos))  # autocorrelation is even

def loadseries(filename):
    return np.loadtxt(filename, dtype=(np.int, np.int), usecols=(0,1))
