#ifndef __SLON_ENGINE_LOG_DETAIL_LOGGER__
#define __SLON_ENGINE_LOG_DETAIL_LOGGER__

#include "../Logger.h"
#include "Stream.h"
#include <boost/intrusive_ptr.hpp>

namespace slon {
namespace log {
namespace detail {

class logger_output;
typedef boost::intrusive_ptr<logger_output> logger_output_ptr;

class logger_output :
    public referenced
{
public:
    typedef boost::shared_ptr<std::filebuf> filebuf_ptr;
    typedef boost::shared_ptr<ostream>      ostream_ptr;
    typedef std::vector<logger_output*>	    logger_output_vector;

public:
    // construct output, redirect to cout
    logger_output();

    // construct output, redirect to parent output
    logger_output(logger_output* _parent, const std::string& _name);

    ~logger_output();

public:
    // tree
    logger_output_ptr		parent;
    logger_output_vector    children;
    std::string             name;

    // io
    filebuf_ptr fb;
    ostream_ptr os;
};

class Logger :
	public log::Logger
{
public:
	Logger(const logger_output_ptr& loggerOutput);

   	// Override Logger
	const std::string&	getName() const	{ return loggerOutput->name; }
	void				flush()			{ loggerOutput->os->flush(); }

	std::ostream& operator << (SEVERITY severity);

	/** Get internal ostream holder */
	logger_output_ptr getLoggerOutput() { return loggerOutput; }

private:
    logger_output_ptr loggerOutput;
};

typedef boost::intrusive_ptr<Logger>		logger_ptr;
typedef boost::intrusive_ptr<const Logger>	const_logger_ptr;

} // namespace detail
} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_DETAIL_LOGGER__
