#ifndef __SLON_ENGINE_UTILITY_TMP_FILE_HPP__
#define __SLON_ENGINE_UTILITY_TMP_FILE_HPP__

#include <fstream>
#include <ios>
#include <stdio.h>

namespace slon {

/** Temporary file, erases file from disc in destructor.
 * NOTE: You have to open and close tmp_file as regular std::ostream
 * WARNING: doesn't cooperate with SlonEngine filesystem.
 */
template<typename Elem, typename Traits>
class basic_tmp_file :
	public std::basic_ofstream<Elem, Traits>
{
public:
    typedef std::basic_ofstream<Elem, Traits> base_type;

public:
	basic_tmp_file(const char* dir = "")
	:	name_(dir)
	{
		if ( !name_.empty() && *name_.rbegin() != '\\' && *name_.rbegin() != '/' ) {
			name_ += "/";
		}
		name_ += tmpnam(0);
	}

    using std::basic_ofstream<Elem, Traits>::open;

    bool open(std::ios_base::openmode mode = std::ios_base::out)
    {
        base_type::open( name_.c_str(), mode );
        return base_type::is_open();
    }

    ~basic_tmp_file()
    {
        base_type::close();
        remove(name_.c_str());
    }

	const std::string& name() const { return name_; }

private:
	std::string name_;
};

typedef basic_tmp_file<char, std::char_traits<char> >		tmp_file;
typedef basic_tmp_file<wchar_t, std::char_traits<wchar_t> >	wtmp_file;

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_TMP_FILE_HPP__
