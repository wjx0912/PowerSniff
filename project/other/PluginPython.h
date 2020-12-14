#pragma once

void CheckLibPython();

// python35.dllµ¼³öº¯Êý
typedef void (*Py_Initialize1)(void);
typedef void (*Py_Finalize1)(void);
typedef void (*Py_SetPythonHome1)(wchar_t *home);
extern Py_Initialize1				Py_Initialize;
extern Py_Finalize1				Py_Finalize;
extern Py_SetPythonHome1			Py_SetPythonHome;
