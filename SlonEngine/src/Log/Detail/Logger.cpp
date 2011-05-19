#include "stdafx.h"
#include "Log/Detail/Logger.h"
#include "Log/Detail/Stream.h"

namespace slon {
namespace log {
namespace detail {

// construct output, redirect to cout
logger_output::logger_output()
:   parent(0)
,	os( new log::ostream( logger_sink( std::cout.rdbuf() ) ) )
{
    os->setf(std::ios_base::unitbuf);
}

// construct output, redirect to parent output
logger_output::logger_output(logger_output* _parent, const std::string& _name) :
    parent(_parent),
    name(_name),
    os(_parent->os)
{
    parent->children.push_back(this);
}

logger_output::~logger_output()
{
    if (parent) {
		std::remove(parent->children.begin(), parent->children.end(), this);
    }
	for (size_t i = 0; i<children.size(); ++i) {
		children[i]->parent = 0;
	}
}

Logger::Logger(const logger_output_ptr& loggerOutput_) :
    loggerOutput(loggerOutput_)
{
}

std::ostream& Logger::operator << (SEVERITY severity)
{
    using namespace slon::log;

    assert(loggerOutput);
    size_t flags = (*loggerOutput->os)->get_flags();
    if ( !(flags & logger_sink::SKIP_INFO) )
    {
    switch(severity)
    {
        case log::S_FLOOD:
            *(loggerOutput->os) << loggerOutput->name << " [flood] - ";
            break;

        case log::S_NOTICE:
            *(loggerOutput->os) << loggerOutput->name << " [notify] - ";
            break;

        case log::S_WARNING:
            *(loggerOutput->os) << loggerOutput->name << " [warning] - ";
            break;

        case log::S_ERROR:
            *(loggerOutput->os) << loggerOutput->name << " [error] - ";
            break;

        case log::S_FATAL_ERROR:
            *(loggerOutput->os) << loggerOutput->name << " [fatal error] - ";
            break;

        default:
            assert(!"Invalid case condition");
            break;
        }
    }

    return *(loggerOutput->os);
}

} // namespace detail
} // namespace slon
} // namespace log
