#ifndef __SLON_ENGINE_LOG_LOGGER__
#define __SLON_ENGINE_LOG_LOGGER__

#include "../Utility/referenced.hpp"
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define __DEFINE_LOGGER__(name) namespace { slon::log::Logger logger(name); }

#ifdef _DEBUG
#   define LOG_FILE_AND_LINE "; in" << __FILE__ << " at " << __LINE__ << " line"
#else
#   define LOG_FILE_AND_LINE ""
#endif

namespace slon {
namespace log {

enum WARNING_LEVEL
{
    WL_NOTIFY,     /// Just notify about some notisable events
    WL_WARNING,    /// Warn user
    WL_ERROR,      /// Report an error, that engine will resolve.
    WL_FATAL_ERROR /// Report an error, that engine can't resolve.
};

/** Logger is simple ostream that redirects output
 * to the specified by LogManager destination. Loggers
 * are organized in tree structure. Id you redirect output
 * of the parent logger, all childs will be redirected too.
 */
class Logger
{
friend class LogManager;
public:
    struct  logger_output;
    typedef boost::intrusive_ptr<logger_output> logger_output_ptr;
    typedef boost::shared_ptr<std::filebuf>     filebuf_ptr;
    typedef boost::shared_ptr<std::ostream>     ostream_ptr;

    typedef std::vector<logger_output*>         logger_output_vector;

    struct logger_output :
        public referenced
    {
        // tree
        logger_output_ptr       parent;
        logger_output_vector    children;
        std::string             name;

        // io
        filebuf_ptr fb;
        ostream_ptr os;

        // construct output, redirect to cout
        logger_output() :
            os( new std::ostream( std::cout.rdbuf() ) )
        {
            os->setf(std::ios_base::unitbuf);
        }

        // construct output, redirect to parent output
        logger_output(const logger_output_ptr& _parent, const std::string& _name) :
            parent(_parent),
            name(_name),
            os(_parent->os)
        {
            parent->children.push_back(this);
        }

        ~logger_output()
        {
            if (parent) {
                parent->children.erase( std::find(parent->children.begin(), parent->children.end(), this) );
            }
        }
    };

private:
    // construct logger without registering it
    Logger();

public:
    /** Create logger.
     * @param name - name of the logger. Log manager uses it
     * to define logger output.
     */
    Logger(const std::string& name);

    /** Get name of the logger. */
    std::string getName() const { return loggerOutput->name; }

    /** Write warning level and log name. */
    std::ostream& operator << (WARNING_LEVEL warningLevel);

    /** Flush as ostream. */
    void flush() { loggerOutput->os->flush(); }

private:
    logger_output_ptr loggerOutput;
};

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_LOGGER__
