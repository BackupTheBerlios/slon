#include "stdafx.h"
#include "Thread/Utility.h"

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace slon {
namespace thread {

void sleep(double time)
{
#ifdef WIN32
    Sleep( static_cast<DWORD>(time * 1000.0) );
#else
    usleep( static_cast<useconds_t>(time * 1000000.0) );
#endif
}

} // namespace thread
} // namespace slon
