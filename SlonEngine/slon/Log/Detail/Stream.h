#ifndef __SLON_ENGINE_LOG_DETAIL_STREAM__
#define __SLON_ENGINE_LOG_DETAIL_STREAM__

#include "../Formatters.h"
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>
#include <stack>

namespace slon {
namespace log {

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

    #if defined(_DEBUG) && defined(MSVC)
        if (written > 0)
        {
            std::string msg(s, written);
            OutputDebugStringA(msg.c_str());
        }
    #endif

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

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_DETAIL_STREAM__
