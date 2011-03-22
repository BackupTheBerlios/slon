#include "stdafx.h"
#include "Utility/error.hpp"

namespace {

    std::string formatSystemError(const std::string& prefix, int errorCode)
    {
        std::string errorMsg = prefix;
    #ifdef WIN32
        static const DWORD ERROR_BUFFER_MAX_SIZE = 256;
        static CHAR        errorBuffer[ERROR_BUFFER_MAX_SIZE];

        if ( FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, LANG_SYSTEM_DEFAULT, errorBuffer, ERROR_BUFFER_MAX_SIZE, NULL) ) {
            errorMsg += errorBuffer;
        }
        else {
            errorMsg += "unknown";
        }
    #elif defined(__linux__)
    #endif
        return errorMsg;
    }

} // anonymous namespace

namespace slon {

slon_error::slon_error(const std::string& message)
:   std::runtime_error(message)
{
}

slon_error::slon_error( log::Logger&         logger,
                        const std::string&   message,
                        log::SEVERITY   severity )
:   std::runtime_error(message)
{
    logger << severity << message << std::endl;
    logger.flush(); // need immediate result
}

system_error::system_error( log::Logger&         logger,
                            const std::string&   message,
                            log::SEVERITY   severity )
:   slon_error(logger, message, severity)
{
}

system_error::system_error( log::Logger&         logger,
                            const std::string&   message,
                            int                  errorCode,
                            log::SEVERITY   severity )
:   slon_error(logger, formatSystemError(message, errorCode), severity)
{
}

file_not_found_error::file_not_found_error( log::Logger&         logger,
                                            const std::string&   message,
                                            log::SEVERITY   severity )
:   slon_error(logger, message, severity)
{
}

io_error::io_error( log::Logger&         logger,
                    const std::string&   message,
                    log::SEVERITY   severity )
:   slon_error(logger, message, severity)
{
}

namespace graphics {

shader_error::shader_error( log::Logger&          logger,
                            const std::string&    message,
                            log::SEVERITY    severity )
:   slon_error(logger, message, severity)
{
}

unsupported_error::unsupported_error( log::Logger&         logger,
                                      const std::string&   message,
                                      log::SEVERITY   severity )
:   slon_error(logger, message, severity)
{
}

gl_error::gl_error( log::Logger&         logger,
                    const std::string&   message,
                    log::SEVERITY   severity )
:   slon_error(logger, message, severity)
{
}

} // namespace graphics
} // namespace slon