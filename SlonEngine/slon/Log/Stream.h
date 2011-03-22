#ifndef __SLON_ENGINE_LOG_STREAM__
#define __SLON_ENGINE_LOG_STREAM__

#include "Forward.h"
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>
#include <stack>

namespace slon {
namespace log {
	
/** Setup indent filter to the ostream */
struct indent
{
	indent(int n_ = 4, char ch_ = ' ')
	:	n(n_)
	,	ch(ch_)
	{}

	int  n;
	char ch;
};

/** Remove previous indent manipulator */
struct unindent
{};

struct skip_info
{
    explicit skip_info(bool skip_)
    :   skip(skip_)
    {}

    bool skip;
};

class logger_sink : 
	public boost::iostreams::sink 
{
public:
    enum
    {
        SKIP_INFO = 1
    };

public:
	explicit logger_sink(std::streambuf* buf_)
	:	buf(buf_)
    ,   flags(0)
    ,   newline(true)
    ,   toIndent(0)
	,	indentSize(0)
	,	indentChar(' ')
	{}

    std::streamsize write(const char* s, std::streamsize n)
	{
		std::streamsize written = 0;
		while (n > 0)
		{
			while (toIndent > 0)
			{
				if (buf->sputc(indentChar) == EOF) {
					return written;
				}
                --toIndent;
			}

            char c = *s;
            {
                // putchar
                if (buf->sputc(c) == EOF) {
                    return written;
                }
			    ++written;

                // setup indentation if necessary
			    if (c == '\n') 
                {
                    newline = true;
                    toIndent = indentSize;
			    }
                else {
                    newline = false;
                }
            }

			++s; --n;
		}

		return written;
	}
	
	void set_indent(const indent& it)
	{
		indents.push(it);
		indentSize += it.n;
		indentChar  = it.ch;
        if (newline) {
			toIndent = indentSize;
        }
	}

	void unindent()
	{
		const indent& it = indents.top(); indents.pop();
		indentSize -= it.n;
		indentChar  = it.ch;
        if (newline) {
			toIndent = indentSize;
        }
	}

    void set_flags(size_t flags_)
    {
        flags = flags_;
    }

    size_t get_flags()
    {
        return flags;
    }

private:
	std::streambuf*		buf;
	std::stack<indent>	indents;
    size_t              flags;
    bool                newline;
    size_t              toIndent;
	size_t				indentSize;
	char_type			indentChar;
};

typedef boost::iostreams::stream<logger_sink> ostream;

inline log::ostream& operator << (log::ostream& os, const indent& it)
{
	os.flush();
	(*os).set_indent(it);
	return os;
}

inline std::ostream& operator << (std::ostream& os, const indent& it)
{
	if ( log::ostream* fos = dynamic_cast<log::ostream*>(&os) ) {
		*fos << it;
	}

	return os;
}

inline log::ostream& operator << (log::ostream& os, const unindent&)
{
	os.flush();
	(*os).unindent();	
	return os;
}

inline std::ostream& operator << (std::ostream& os, const unindent& uit)
{
	if ( log::ostream* fos = dynamic_cast<log::ostream*>(&os) ) {
		*fos << uit;
	}

	return os;
}

inline log::ostream& operator << (log::ostream& os, const skip_info& si)
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

inline std::ostream& operator << (std::ostream& os, const skip_info& si)
{
	if ( log::ostream* fos = dynamic_cast<log::ostream*>(&os) ) {
		*fos << si;
	}

	return os;
}

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_STREAM__
