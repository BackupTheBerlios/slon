#include "stdafx.h"
#include "Script/Detail/ScriptManager.h"
#ifdef SLON_ENGINE_USE_PYTHON
#   include <Python.h>
#endif

namespace slon {
namespace script {
namespace detail {

ScriptManager::ScriptManager()
{
#ifdef SLON_ENGINE_USE_PYTHON
    Py_Initialize();
#endif
}

ScriptManager::~ScriptManager()
{
#ifdef SLON_ENGINE_USE_PYTHON
    Py_Finalize();
#endif
}

void ScriptManager::exec(const char* scriptFile, const char* cmdLine)
{
}

} // namespace detail
} // namespace script
} // namespace slon
