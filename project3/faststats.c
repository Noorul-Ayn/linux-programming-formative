/*
 * faststats.c
 * A CPython extension module that computes descriptive statistics
 * on a list of numbers in C. It implements the same algorithm as
 * purestats.py (two passes: mean, then variance) so that the
 * benchmark compares language overhead, not algorithms.
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>

/**
 * faststats_compute_stats - computes count, mean, variance,
 * standard deviation, min and max of a Python list of numbers.
 *
 * Strategy: convert the Python list into a plain C double array
 * once, then do all arithmetic in C with no interpreter involvement.
 */
static PyObject *faststats_compute_stats(PyObject *self, PyObject *args)
{
	PyObject *input_list;
	double *values;
	double total = 0.0, mean, sq_diff_sum = 0.0, variance, std_dev;
	double minimum, maximum, diff;
	Py_ssize_t n, i;

	(void)self;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &input_list))
		return (NULL);

	n = PyList_Size(input_list);
	if (n == 0)
	{
		PyErr_SetString(PyExc_ValueError, "empty data");
		return (NULL);
	}

	values = (double *)malloc((size_t)n * sizeof(double));
	if (values == NULL)
		return (PyErr_NoMemory());

	/* Pass 1: convert Python objects to C doubles once, and
	 * accumulate sum, min and max in the same loop */
	minimum = HUGE_VAL;
	maximum = -HUGE_VAL;
	for (i = 0; i < n; i++)
	{
		values[i] = PyFloat_AsDouble(PyList_GET_ITEM(input_list, i));
		if (values[i] == -1.0 && PyErr_Occurred())
		{
			free(values);
			return (NULL);
		}
		total += values[i];
		if (values[i] < minimum)
			minimum = values[i];
		if (values[i] > maximum)
			maximum = values[i];
	}
	mean = total / (double)n;

	/* Pass 2: variance */
	for (i = 0; i < n; i++)
	{
		diff = values[i] - mean;
		sq_diff_sum += diff * diff;
	}
	variance = sq_diff_sum / (double)n;
	std_dev = sqrt(variance);

	free(values);

	return (Py_BuildValue("{s:n,s:d,s:d,s:d,s:d,s:d}",
			      "count", n,
			      "mean", mean,
			      "variance", variance,
			      "std_dev", std_dev,
			      "min", minimum,
			      "max", maximum));
}

static PyMethodDef FastStatsMethods[] = {
	{"compute_stats", faststats_compute_stats, METH_VARARGS,
	 "Compute descriptive statistics of a list of numbers in C."},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef faststatsmodule = {
	PyModuleDef_HEAD_INIT,
	"faststats",
	"C extension computing descriptive statistics on large arrays.",
	-1,
	FastStatsMethods,
	NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC PyInit_faststats(void)
{
	return (PyModule_Create(&faststatsmodule));
}
