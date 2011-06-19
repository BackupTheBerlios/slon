#ifndef __SLON_ENGINE_STDAFX_H__
#define __SLON_ENGINE_STDAFX_H__

// stdlib
#include <cstdlib>

// STL
#include <algorithm>
#include <deque>
#include <functional>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <queue>
#include <vector>

// BOOST
#include <boost/bind.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/type_traits.hpp>
#include <boost/variant.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/xpressive/xpressive.hpp>

// System
#ifdef WIN32
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#elif defined(__linux__)
#endif

#endif // __SLON_ENGINE_STDAFX_H__