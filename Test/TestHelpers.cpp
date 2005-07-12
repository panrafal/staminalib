#include <stdafx.h>

#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "..\Helpers.h"

using namespace Stamina;

using std::string;
using namespace std;

#include <Stamina\Console.h>

class TestHelpers : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestHelpers );
  
	CPPUNIT_TEST( testFind_noCase );

    CPPUNIT_TEST_SUITE_END();

protected:
  

public:
	void setUp() {
	}
	void tearDown() {
	}

protected:


	void testFind_noCase() {
		CPPUNIT_ASSERT_EQUAL((size_t)3, find_noCase("123¹æœ123", "¥æŒ"));
		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase("123¹æœ123", "c"));
		CPPUNIT_ASSERT_EQUAL((size_t)5, find_noCase("123¹æ¥Æœ123", "¥æŒ"));
		CPPUNIT_ASSERT_EQUAL((size_t)3, find_noCase(L"123¹æœ123", L"¥æŒ"));
		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase(L"123¹æœ123", L"c"));
		CPPUNIT_ASSERT_EQUAL((size_t)5, find_noCase(L"123¹æ¥Æœ123", L"¥æŒ"));

		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase("123¹æœ123", ""));
		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase("123¹æœ123", " 123¹æœ123 "));
		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase("", "a"));
	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestHelpers );


