#include "stdafx.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Scene/Group.h"

namespace slon {
namespace log {

LogVisitor::LogVisitor(const log::logger_ptr&	logger_, 
                       log::SEVERITY			severity_)
:   logger(logger_)
,   severity(severity_)
{
	assert(logger);
}

LogVisitor::LogVisitor(const log::logger_ptr&	logger_, 
                       log::SEVERITY			severity_, 
                       const scene::Node&		node)
:   logger(logger_)
,   severity(severity_)
{
    if (logger) {
		traverse(node);
	}
}

void LogVisitor::traverse(const scene::Node& node)
{
    assert(logger);
    *logger << severity << "====================== begin traverse ======================\n" << log::skip_info(true);
    node.accept(*this);
    *logger << severity << "======================= end traverse =======================\n" << log::skip_info(false);
}

void LogVisitor::visitGroup(const scene::Group& group)
{
    for (const scene::Node* i = group.getChild(); i; i = i->getRight())
    {
        i->accept(*this);
    }
}

} // namepsace log
} // namespace slon
