#ifndef __SLON_ENGINE_ERROR_H__
#define __SLON_ENGINE_ERROR_H__

#include "../Log/Logger.h"
#include <stdexcept>

namespace slon {

/** Base class for slon engine errors */
class slon_error :
    public std::runtime_error
{
public:
    slon_error( const std::string&   message);

    slon_error( log::Logger&         logger,
                const std::string&   message,
                log::WARNING_LEVEL   warningLevel = log::WL_ERROR );

	~slon_error() throw() {} 
};

/** Error occured when some system function fails. Mose common reason - invalid parameters. */
class system_error :
    public slon_error
{
public:
    system_error( log::Logger&         logger,
                  const std::string&   message,
                  log::WARNING_LEVEL   warningLevel = log::WL_ERROR );

    system_error( log::Logger&         logger,
                  const std::string&   message,
                  int                  errorCode,
                  log::WARNING_LEVEL   warningLevel = log::WL_ERROR );

	~system_error() throw() {} 
};

/** Error occured when necessary file is not found. */
class file_not_found_error :
    public slon_error
{
public:
    file_not_found_error( log::Logger&         logger,
                          const std::string&   message,
                          log::WARNING_LEVEL   warningLevel = log::WL_ERROR );

	~file_not_found_error() throw() {} 
};

/** Error occures due to failed input/output operation */
class io_error :
    public slon_error
{
public:
    io_error( log::Logger&         logger,
              const std::string&   message,
              log::WARNING_LEVEL   warningLevel = log::WL_ERROR );

	~io_error() throw() {} 
};

namespace graphics {

    /** Error occured during loading, compiling or linking shaders. */
    class shader_error :
        public slon_error
    {
    public:
        shader_error( log::Logger&          logger,
                      const std::string&    message,
                      log::WARNING_LEVEL    warningLevel = log::WL_ERROR );
		
		~shader_error() throw() {} 
    };

    /** Error occured if device doesn't support requested feature. */
    class unsupported_error :
        public slon_error
    {
    public:
        unsupported_error( log::Logger&         logger,
                           const std::string&   message,
                           log::WARNING_LEVEL   warningLevel = log::WL_ERROR );

		~unsupported_error() throw() {} 
    };

    /** Error occured if graphics library can't perform necessary actions. */
    class gl_error :
        public slon_error
    {
    public:
        gl_error( log::Logger&         logger,
                  const std::string&   message,
                  log::WARNING_LEVEL   warningLevel = log::WL_ERROR );

		~gl_error() throw() {} 
    };
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_ERROR_H__
