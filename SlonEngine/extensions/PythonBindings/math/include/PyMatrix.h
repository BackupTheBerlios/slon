#ifndef __PY_MATRIX_INCLUDED__
#define __PY_MATRIX_INCLUDED__

#include <python.h>

inline void IndexError() 
{ 
    PyErr_SetString(PyExc_IndexError, "Index out of range");
}

void exportMatrix();

#endif // __PY_MATRIX_INCLUDED__