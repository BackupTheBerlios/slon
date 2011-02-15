#pragma once

namespace slon {

// May be replace with some 3rd-party template 
template<bool B, typename T1, typename T2>
struct if_then_else
{
	typedef T1 type;
};

template<typename T1, typename T2>
struct if_then_else<false, T1, T2>
{
	typedef T2 type;
};

} // namespace slon