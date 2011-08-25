#ifndef __SLON_ENGINE_SCRIPT_DETAIL_SCRIPT_MANAGER_H__
#define __SLON_ENGINE_SCRIPT_DETAIL_SCRIPT_MANAGER_H__

#include "../ScriptManager.h"

namespace slon {
namespace script {
namespace detail {

class ScriptManager :
    public script::ScriptManager
{
public:
    ScriptManager();
    ~ScriptManager();

    // Override ScriptManager
    void exec(const char* scriptFile, const char* cmdLine = 0);
};

} // namespace detail
} // namespace script
} // namespace slon

#endif // __SLON_ENGINE_SCRIPT_DETAIL_SCRIPT_MANAGER_H__
