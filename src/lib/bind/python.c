#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "lib/bind/python.h"

// from https://docs.python.org/3/extending/embedding.html
bool run_python(
	const char *directory,
	const char *python_filename,
	const char *fname,
	unsigned    part,
	const char *input_filename)
{
	if (!directory || !python_filename || !fname || !input_filename)
	{
		fprintf(stderr, "missing params - run python\n");
		return false;
	}

	PyObject *pName, *pModule, *pFunc;
	PyObject *pArgs, *pValue;
	bool success = false;

	wchar_t program_name[
		strlen(directory) + strlen(python_filename) + 1];
	swprintf(program_name, 100, L"%hs%hs", directory, python_filename);

	Py_SetProgramName(program_name);
	Py_Initialize();

	wchar_t *argv[] = { program_name };
	PySys_SetArgv(1, argv);

	pName = PyUnicode_DecodeFSDefault(python_filename);
	/* Error checking of pName left out */

	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule != NULL) {
		pFunc = PyObject_GetAttrString(pModule, fname);
		/* pFunc is a new reference */

		if (pFunc && PyCallable_Check(pFunc)) {
			pArgs = PyTuple_New(2);

			pValue = PyLong_FromLong(part);
			PyTuple_SetItem(pArgs, 0, pValue);

			if (!pValue) {
				Py_DECREF(pArgs);
				Py_DECREF(pModule);
				fprintf(stderr, "Cannot convert argument\n");
				return false;
			}

			pValue = PyUnicode_FromString(input_filename);
			PyTuple_SetItem(pArgs, 1, pValue);

			if (!pValue) {
				Py_DECREF(pArgs);
				Py_DECREF(pModule);
				fprintf(stderr, "Cannot convert argument\n");
				return false;
			}

			pValue = PyObject_CallObject(pFunc, pArgs);
			Py_DECREF(pArgs);
			if (pValue != NULL) {
				long result = PyLong_AsLong(pValue);
				printf("Result of call: %ld\n", result);
				success = result != 0;
				Py_DECREF(pValue);
			}
			else {
				Py_DECREF(pFunc);
				Py_DECREF(pModule);
				PyErr_Print();
				fprintf(stderr, "Call failed\n");
				return false;
			}
		}
		else {
			if (PyErr_Occurred())
				PyErr_Print();
			fprintf(stderr, "Cannot find function \"%s\"\n", fname);
		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);
	}
	else {
		PyErr_Print();
		fprintf(stderr, "Failed to load \"%s\"\n", python_filename);
		return false;
	}
	if (Py_FinalizeEx() < 0) {
		// ???
		return false;
	}
	return success;
}
