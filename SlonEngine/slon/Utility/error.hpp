#ifndef __SLON_ENGINE_ERROR_H__
#define __SLON_ENGINE_ERROR_H__

#include "../Log/Logger.h"
#include <stdexcept>
#ifdef __GNUC__
#   include <signal.h>
#endif
#ifdef WIN32
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

namespace slon {

/** Stop application, raise debugger. Platform independent variant for msvs DebugBreak() */
inline void debug_break()
{
#ifdef _MSC_VER
	DebugBreak();
#elif __linux__
	raise(SIGTRAP);
#else
	assert(!"Debug break");
#endif
}

/** Base class for slon engine errors */
class slon_error :
    public std::runtime_error
{
public:
    slon_error( const std::string&		message);

    slon_error( const log::logger_ptr&  logger,
                const std::string&		message,
                log::SEVERITY			severity = log::S_ERROR );

	~slon_error() throw() {} 
};

/** Error occured when some system function fails. Mose common reason - invalid parameters. */
class system_error :
    public slon_error
{
public:
    system_error( const log::logger_ptr&    logger,
                  const std::string&		message,
                  log::SEVERITY				severity = log::S_ERROR );

    system_error( const log::logger_ptr&    logger,
                  const std::string&		message,
                  int						errorCode,
                  log::SEVERITY				severity = log::S_ERROR );

	~system_error() throw() {} 
};

/** Error occured when necessary file is not found or error occured opening it. */
class file_error :
    public slon_error
{
public:
    file_error( const log::logger_ptr&	logger,
                const std::string&		message,
                log::SEVERITY				severity = log::S_ERROR );

	~file_error() throw() {} 
};

/** Error occures due to failed input/output operation */
class io_error :
    public slon_error
{
public:
    io_error( const log::logger_ptr&    logger,
              const std::string&		message,
              log::SEVERITY				severity = log::S_ERROR );

	~io_error() throw() {} 
};

namespace database {

    class serialization_error :
        public slon_error
    {
    public:
        serialization_error( const std::string&		message );

        serialization_error( const log::logger_ptr&	logger,
                             const std::string&		message,
                             log::SEVERITY          severity = log::S_ERROR );

	    ~serialization_error() throw() {} 
    };

} // namespace database

namespace graphics {

    /** Error occured during loading, compiling or linking shaders. */
    class shader_error :
        public slon_error
    {
    public:
        shader_error( const log::logger_ptr&    logger,
                      const std::string&		message,
                      log::SEVERITY				severity = log::S_ERROR );
		
		~shader_error() throw() {} 
    };

    /** Error occured if device doesn't support requested feature. */
    class unsupported_error :
        public slon_error
    {
    public:
        unsupported_error( const log::logger_ptr&   logger,
                           const std::string&		message,
                           log::SEVERITY			severity = log::S_ERROR );

		~unsupported_error() throw() {} 
    };

    /** Error occured if graphics library can't perform necessary actions. */
    class gl_error :
        public slon_error
    {
    public:
        gl_error( const log::logger_ptr&    logger,
                  const std::string&		message,
                  log::SEVERITY				severity = log::S_ERROR );

		~gl_error() throw() {} 
    };
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_ERROR_H__
