#ifndef __SLON_ENGINE_UTILITY_URI_HPP__
#define __SLON_ENGINE_UTILITY_URI_HPP__

#include <boost/xpressive/xpressive.hpp>

namespace slon {

template<typename CharType>
class uri
{
public:
	typedef CharType					char_t;
	typedef std::basic_string<char_t>	string_t;

protected:
	typedef boost::xpressive::basic_regex<typename string_t::const_iterator>	regex_t;
	typedef boost::xpressive::match_results<typename string_t::const_iterator>	match_t;

public:
	uri() : isValid(false) {}
	uri(const string_t& uri_) {	from_string(uri_); }

	virtual bool from_string(const string_t& str)
	{
		using namespace boost::xpressive;

		static const regex_t scheme_regexp = !((s1 = *(_w | _d | '+' | '-' | '.')) >> ':') // scheme
										   >> (s2 = *_)									   // hierarchical
										   >> !('?' >> (s3 = *_))						   // query
										   >> !('#' >> (s4 = *_));						   // fragment

		match_t match;
		if ( regex_match(str, match, scheme_regexp) ) 
		{
			scheme       = match[1];
			hierarchical = match[2];
			query        = match[3];
			fragment     = match[4];
			isValid      = true;
		}
		else {
			isValid = false;
		}

		return isValid;
	}

	virtual string_t to_string() const
	{
		string_t res = scheme + ":" + hierarchical;
		if ( !query.empty() ) {
			res += "?" + query;
		}
		if ( !fragment.empty() ) {
			res += "#" + fragment;
		}
		return res;
	}

	bool valid() const { return isValid; }

protected:
	bool	 isValid;

public:
	string_t scheme;
	string_t hierarchical;
	string_t query;
	string_t fragment;
};

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_URI_HPP__