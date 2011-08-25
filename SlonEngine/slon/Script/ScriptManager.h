#ifndef __SLON_ENGINE_SCRIPT_SCRIPT_MANAGER_H__
#define __SLON_ENGINE_SCRIPT_SCRIPT_MANAGER_H__

#include "../Config.h"

namespace slon {
namespace script {

class ScriptManager
{
public:
    /** Execute script file:
     * @param scriptFile - script file name in virtual file system.
     * @param cmdLine - command line parameters.
     */
    virtual void exec(const char* scriptFile, const char* cmdLine = 0) = 0;

protected:
    ~ScriptManager() {}
};

} // namespace script
} // namespace slon

#endif // __SLON_ENGINE_SCRIPT_SCRIPT_MANAGER_H__
