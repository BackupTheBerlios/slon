#ifndef __SLON_ENGINE_LOG_MANIPULATORS__
#define __SLON_ENGINE_LOG_MANIPULATORS__

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
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

class logger_sink : 
	public boost::iostreams::sink 
{
public:
	logger_sink(std::streambuf* buf_)
	:	buf(buf_)
	,	indentSize(0)
	,	indentChar(' ')
	{}

    std::streamsize write(const char* s, std::streamsize n)
	{
		std::streamsize written = 0;
		while ( n > 0 && buf->sputc(*s) != EOF )
		{
			++written;

			if (*s == '\n')
			{
				for (size_t i = 0; i<indentSize; ++i) 
				{
					if (buf->sputc(indentChar) == EOF) {
						return written;
					}
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
	}

	void unindent()
	{
		const indent& it = indents.top();
		indentSize -= it.n;
		indentChar  = it.ch;
		indents.pop();
	}

private:
	std::streambuf*		buf;
	std::stack<indent>	indents;
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

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_MANIPULATORS__
