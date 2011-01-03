#ifndef __SLON_ENGINE_UTILITY_URI_FILE_URI_HPP__
#define __SLON_ENGINE_UTILITY_URI_FILE_URI_HPP__

#include "uri.hpp"

namespace slon {

template<typename CharType>
class file_uri :
	public uri<CharType>
{
public:
    typedef uri<CharType>                   base_type;
    typedef typename base_type::string_t    string_t;

public:
	file_uri() {}
	file_uri(const string_t& uri_) { from_string(uri_); }

	virtual bool from_string(const string_t& str)
	{
		using namespace boost::xpressive;

        if ( base_type::from_string(str) )
		{
            if (base_type::scheme == "file")
			{
                if (base_type::hierarchical.substr(0, 3) != "///")
				{
                    host = base_type::hierarchical.substr(2);
					size_t slashId = host.find('/');
					if (slashId != string_t::npos) {
						host = host.substr(0, slashId);
					}
				#ifdef _WIN32
                    path = base_type::hierarchical.substr(host.size() + 1);
				#else
                    path = base_type::hierarchical.substr(host.size());
				#endif
				}
				else 
				{
				#ifdef _WIN32
                    path = base_type::hierarchical.substr(3);
				#else
                    path = base_type::hierarchical.substr(2);
				#endif
				}
			}
			else {
                base_type::isValid = false;
			}
		}

        return base_type::isValid;
	}

	virtual string_t to_string() const
	{
		string_t res = "file://";
		if ( !host.empty() ) {
			res += host;
		}
	#ifdef _WIN32
		res += "/";
	#endif
		res += path;
		return res;
	}

public:
	string_t	host;
	string_t	path;
};

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_URI_FILE_URI_HPP__
