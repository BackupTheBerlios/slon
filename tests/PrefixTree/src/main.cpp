#include "Utility/unique_string.hpp"
#include <map>

#define BOOST_TEST_MODULE PrefixTreeTest
#ifndef _UNICODE
#define BOOST_TEST_MAIN
#endif
#include <boost/test/unit_test.hpp>

// MSVS 2010 fix
#if defined(_UNICODE) && defined(_MSC_VER) && __MSCVER >= 1600
int _tmain(int argc, wchar_t * argv[])
{
    char ** utf8Lines;
    int returnValue;

    //Allocate enough pointers to hold the # of command items (+1 for a null line on the end)
    utf8Lines = new char* [argc + 1];

    //Put the null line on the end (Ansi stuff...)
    utf8Lines[argc] = new char[1];
    utf8Lines[argc][0] = NULL;

    //Convert commands into UTF8 for non wide character supporting boost library
    for(unsigned int idx = 0; idx < argc; idx++)
    {
        int convertedLength;
        convertedLength = WideCharToMultiByte(CP_UTF8, NULL, argv[idx], -1, NULL, NULL, NULL, NULL);
        if (convertedLength == 0)
                return GetLastError();
        utf8Lines[idx] = new char[convertedLength]; // WideCharToMultiByte handles null term issues
        WideCharToMultiByte(CP_UTF8, NULL, argv[idx], -1, utf8Lines[idx], convertedLength, NULL, NULL);
    }

    //From boost::test's main()
    returnValue = ::boost::unit_test::unit_test_main( &init_unit_test, argc, utf8Lines );
    //End from boost::test's main()

    //Clean up our mess
    for(unsigned int idx = 0; idx < argc + 1; idx++)
        delete [] utf8Lines[idx];
    delete [] utf8Lines;

    return returnValue;
}
#endif // _UNICODE

template<typename T, typename S>
std::ostream& operator << (std::ostream& s, const std::pair<T, S>& p)
{
	return s << "(" << p.first << ", " << p.second << ")";
}

enum NUMBER
{
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE
};

struct number_traits
{
	static const NUMBER min = ONE;
	static const NUMBER max = NINE;
};

BOOST_AUTO_TEST_CASE(prefix_tree_debug_test)
{
    using namespace slon;

	typedef prefix_tree<NUMBER, int, std::less<NUMBER>, number_traits>	prefix_tree_type;
	typedef prefix_tree_type::key_type									key_type;
	typedef prefix_tree_type::value_type								value_type;

	NUMBER _123[]   = {ONE, TWO, THREE};
	NUMBER _12345[] = {ONE, TWO, THREE, FOUR, FIVE};
	NUMBER _1234[]  = {ONE, TWO, THREE, FOUR};
	NUMBER _345[]   = {THREE, FOUR, FIVE};
	NUMBER _3456[]  = {THREE, FOUR, FIVE, SIX};

	key_type _123k( _123, _123 + sizeof(_123) / sizeof(NUMBER) ); 
	key_type _12345k( _12345, _12345 + sizeof(_12345) / sizeof(NUMBER) ); 
	key_type _1234k( _1234, _1234 + sizeof(_1234) / sizeof(NUMBER) ); 
	key_type _345k( _345, _345 + sizeof(_345) / sizeof(NUMBER) ); 
	key_type _3456k( _3456, _3456 + sizeof(_3456) / sizeof(NUMBER) ); 

	prefix_tree_type prefixTree;
	prefixTree.insert( value_type(_123k, 1) );
	prefixTree.insert( value_type(_12345k, 1) );
	prefixTree.insert( value_type(_1234k, 1) );
	prefixTree.erase(_123k);
	prefixTree.insert( value_type(_345k, 1) );
	prefixTree.erase(_345k);
	prefixTree.insert( value_type(_3456k, 1) );
	prefixTree.erase(_3456k);
}


BOOST_AUTO_TEST_CASE(prefix_tree_validation)
{
    using namespace slon;

    // perform random test
    {
        srand(0);

        static const int TEST_SIZE          = 50000;
        static const int FOR_ERASING        = 10000;
        static const int MAX_STRING_LENGTH  = 50;

        typedef prefix_tree<char, int>						prefix_tree_type;
        typedef std::map<prefix_tree_type::key_type, int>	map_type;

        prefix_tree_type    prefixTree;
        map_type            prefixMap;
        for (int i = 0; i<TEST_SIZE; ++i)
        {
            int         randValue = rand();
            std::string randStr;
            randStr.resize( 1 + rand() % MAX_STRING_LENGTH );
            std::generate(randStr.begin(), randStr.end(), rand);

            bool insTree = prefixTree.insert( prefix_tree_type::value_type(randStr, randValue) ).second;
            bool insMap  = prefixMap.insert( map_type::value_type(randStr, randValue) ).second;
            BOOST_REQUIRE(insTree == insMap);
        }

        // erase something
        for (int i = 0; i<FOR_ERASING; ++i)
        {
            map_type::iterator          iter0 = prefixMap.begin();
            prefix_tree_type::iterator  iter1 = prefixTree.find(iter0->first);
            BOOST_REQUIRE( iter1 != prefixTree.end() );
            prefixMap.erase(iter0);
            prefixTree.erase(iter1);
        }

        // compare sets
        for (map_type::iterator iter  = prefixMap.begin();
                                iter != prefixMap.end();
                                ++iter)
        {
            map_type::iterator          iter0 = prefixMap.begin();
            prefix_tree_type::iterator  iter1 = prefixTree.find(iter0->first);
            BOOST_REQUIRE( iter1 != prefixTree.end() );
            BOOST_CHECK(*iter0 == *iter1);
        }
    }
}

BOOST_AUTO_TEST_CASE(unique_string_validation)
{
    using namespace slon;

    {
        unique_string str1("abacaba");
        unique_string str2("abacaba");
        unique_string str3("abacabadaba");
        unique_string str4("abc");
        unique_string str5("abacabadaba");

        BOOST_CHECK_EQUAL(str1, str2);
        BOOST_CHECK_EQUAL(str3, str5);
        BOOST_CHECK(str2 != str3 && str4 != str5);

        // insert and remove something
        {
            unique_string str1_("a");
            unique_string str2_("daba");
        }

        unique_string str6("abacabadaba");

        BOOST_CHECK_EQUAL(str3, str6);
    }

    {
        unique_string str1("abacaba");
        unique_string str2("abacaba");

        BOOST_CHECK_EQUAL(str1, str2);
    }

    {
        unique_string str1("normalMatrix");
        unique_string str2("normal");

        BOOST_CHECK(str1 != str2);
    }
}