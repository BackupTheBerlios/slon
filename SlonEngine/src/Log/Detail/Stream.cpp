#include "stdafx.h"
#include "Log/Detail/Stream.h"

namespace slon {
namespace log {
	
log::ostream& operator << (log::ostream& os, const indent& it)
{
	os.flush();
	(*os).set_indent(it);
	return os;
}

std::ostream& operator << (std::ostream& os, const indent& it)
{
	if ( log::ostream* fos = dynamic_cast<log::ostream*>(&os) ) {
		*fos << it;
	}

	return os;
}

log::ostream& operator << (log::ostream& os, const unindent&)
{
	os.flush();
	(*os).unindent();	
	return os;
}

std::ostream& operator << (std::ostream& os, const unindent& uit)
{
	if ( log::ostream* fos = dynamic_cast<log::ostream*>(&os) ) {
		*fos << uit;
	}

	return os;
}

log::ostream& operator << (log::ostream& os, const skip_info& si)
{
	os.flush();
    if (si.skip) {
        os->set_flags(os->get_flags() | logger_sink::SKIP_INFO);
    }
    else {
        os->set_flags(os->get_flags() & ~logger_sink::SKIP_INFO);
    }
	return os;
}

std::ostream& operator << (std::ostream& os, const skip_info& si)
{
	if ( log::ostream* fos = dynamic_cast<log::ostream*>(&os) ) {
		*fos << si;
	}

	return os;
}

} // namespace log
} // namespace slon