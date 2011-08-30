#ifndef __PY_MATRIX_INCLUDED__
#define __PY_MATRIX_INCLUDED__

#include <python.h>
#include <sstream>

inline void IndexError(int i, int min, int max) 
{ 
    std::stringstream ss;
    ss << i << " is not in range [" << min << ", " << max << "]";
    PyErr_SetString(PyExc_IndexError, ss.str().c_str());
}

void exportMatrix();

#endif // __PY_MATRIX_INCLUDED__