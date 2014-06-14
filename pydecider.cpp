#include <Python.h>

#include <runner.h>

#ifdef __cplusplus
# define allocate(n,T) ((T*)malloc((n)*sizeof(T)))
#else
# define allocate(n,T) (malloc((n)*sizeof(T)))
#endif

#ifdef __cplusplus
extern "C" {
   static PyObject *
   decider_run(PyObject *self, PyObject *args, PyObject *keywds);

   PyMODINIT_FUNC
   PyInit_decider(void);
}
#endif

static PyObject *
decider_run(PyObject *self, PyObject *args, PyObject *keywds)
{
   PyObject *agentLC = NULL, *machineProbs = NULL;
   PyObject *agentLC_seq = NULL, *machineProbs_seq = NULL;
   double agentLC_default[] = {1.0, 1.0};

   struct RunInfo ri = {
      .agentClass = NULL,
      .agentOrder = -1,
      .agentLC = agentLC_default,
      .agentSeed = 0,
      .machineOrder = -1,
      .machineProbs = NULL,
      .machineSeed = 0,
      .nIter = -1,
      .fileName = NULL,
      .printScore = 0,
      .outputMode = OutputEvolution,
      .nRepetitions = 1,
      .nScore = 0,
      .shiftProbsStep = 0
   };

   int len, i;

   static const char *kwlist[] = {
      // required
      "agentClass", "agentOrder", "machineOrder", "machineProbs",
      "fileName", "nIter",
      // optional
      "nRepetitions", "agentLC", "agentSeed", "machineSeed",
      "outputMode", "printScore", "nScore", "shiftProbsStep",
      NULL};

   ri.agentLC = agentLC_default;
   ri.machineProbs = NULL;

   if (!PyArg_ParseTupleAndKeywords(args, keywds,
         "siiOsi|i$OIIipii", const_cast<char**>(kwlist), 
         &ri.agentClass, &ri.agentOrder, &ri.machineOrder, &machineProbs,
         &ri.fileName, &ri.nIter, &ri.nRepetitions,
         &agentLC, &ri.agentSeed, &ri.machineSeed,
         &ri.outputMode, &ri.printScore, &ri.nScore, &ri.shiftProbsStep))
      return NULL;

   // TODO check if agentClass is valid

   if (ri.agentOrder < 0) {
      PyErr_SetString(PyExc_ValueError, "agentOrder must be zero or greater");
      return NULL;
   }

   if (ri.machineOrder < 0) {
      PyErr_SetString(PyExc_ValueError, "machineOrder must be zero or greater");
      return NULL;
   }

   if (ri.nIter <= 0) {
      PyErr_SetString(PyExc_ValueError, "nIter must be positive");
      return NULL;
   }

   if (ri.nRepetitions <= 0) {
      PyErr_SetString(PyExc_ValueError, "nRepetitions must be positive");
      return NULL;
   }

   if (ri.outputMode >= OutputMode_MAX || ri.outputMode < 0) {
      PyErr_SetString(PyExc_ValueError, "invalid outputMode");
      return NULL;
   }

   // TODO check nScore and shiftProbsStep

   // build machineProbs array

   if (!(machineProbs_seq = PySequence_Fast(machineProbs, "machineProbs must be a sequence")))
      return NULL;

   len = PySequence_Fast_GET_SIZE(machineProbs_seq);
   if (len != 1 << ri.machineOrder) {
      PyErr_SetString(PyExc_ValueError, "machineProbs doesn't have expected size");
      return NULL;
   }

   ri.machineProbs = allocate(len, double);
   for (i = 0; i < len; i++)
      ri.machineProbs[i] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(machineProbs_seq, i));

   // build agentLC array (optional)

   if (agentLC) {
      if (!(agentLC_seq = PySequence_Fast(agentLC, "agentLC must be a sequence")))
         return NULL;

      len = PySequence_Fast_GET_SIZE(agentLC_seq);
      if (len != 2) {
         PyErr_SetString(PyExc_ValueError, "agentLC must have exactly 2 elements");
         return NULL;
      }

      ri.agentLC = allocate(len, double);
      for (i = 0; i < len; i++) {
         ri.agentLC[i] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(agentLC_seq, i));
      }
   }

   runModel(&ri);

   // free built arrays
   if (agentLC)
      free(ri.agentLC);
   free(ri.machineProbs);

   Py_RETURN_NONE;
}

static PyMethodDef DeciderMethods[] = {
   {"run", (PyCFunction)decider_run, METH_VARARGS | METH_KEYWORDS,
      "Run the model with the given parameters (all may be given as\n"
      "keyword arguments):\n"
      "\n"
      "  agentClass -- one of X, XY, WS, Xi, XYi, WSi, WSj, Xb, XYb\n"
      "     (see src/agent.cc)\n"
      "  agentOrder -- agent memory length\n"
      "  machineOrder -- machine memory length\n"
      "  machineProbs -- probabilities\n"
      "  fileName -- output file name\n"
      "  nIter -- number of iterations in each run\n"
      "  nRepetitions (optional) -- number of runs\n"
      "  agentLC (optional) -- list of two coefficients to modulate the\n"
      "     agent's learning\n"
      "  agentSeed (optional) -- fix the random seed for the agent\n"
      "  machineSeed (optional) -- fix the random seed for the machine\n"
      "  outputMode (optional) -- 0, 1, 2 or 3 (see src/runner.h)\n"
      "  printScore (optional)\n"
      "  nScore (optional) -- if provided and nonzero, specifies the maximum\n"
      "     number of trials that should be counted in overall scores (counting\n"
      "     from the last one)\n"
      "  shiftProbsStep (optional)\n"
   },
   {NULL, NULL, 0, NULL}
};

static struct PyModuleDef decider_module = {
   PyModuleDef_HEAD_INIT,
   "decider", /* name of module */
   "Bayesian decision-making algorithm for predicting Markov chains", /* module documentation (may be NULL) */
   -1, /* size of per-interpreter state of the module, or -1 if the module
          keeps state in global variables */
   DeciderMethods
};

PyMODINIT_FUNC
PyInit_decider(void)
{
   return PyModule_Create(&decider_module);
}

