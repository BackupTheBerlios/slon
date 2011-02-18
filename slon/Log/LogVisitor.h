#ifndef __SLON_ENGINE_LOG_LOG_VISITOR_H__
#define __SLON_ENGINE_LOG_LOG_VISITOR_H__

#include "../Scene/Visitors/NodeVisitor.h"
#include "Logger.h"

namespace slon {
namespace log {

/** LogVisitor logs information on the scene graph. Performs traverse using recursion. */
class LogVisitor :
    public scene::ConstNodeVisitor
{
public:
    LogVisitor(log::Logger* logger = 0, log::SEVERITY severity = log::S_FLOOD);
    LogVisitor(log::Logger* logger, log::SEVERITY severity, scene::Node& node);

    /** Traverse scene graph using DFS and native recursion. */
    void traverse(const scene::Node& node);

    // Override NodeVisitor
    void visitGroup(const scene::Group& group);

    /** Get logger */
    log::Logger* getLogger() { return logger; }

    /** Get logger */
    log::Logger* getLogger() const { return logger; }

    /** Get logging severity */
    log::SEVERITY getSeverity() const { return severity; }

    /** Output logger information */
    template<typename T>
    std::ostream& operator << (const T& value)
    {
        if (logger) {
            return *logger << severity << value;
        }
        return std::cerr;
    }

protected:
    log::Logger*   logger;
    log::SEVERITY  severity;
};

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_LOG_VISITOR_H__
