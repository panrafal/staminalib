#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina/VersionControl.h>
#include <Stamina/WideChar.h>
#include <Stamina/StringType.h>
//#include <Stamina/String.h>
#include <ConvertUTF.h>

using namespace Stamina;

template <typename CHAR>
class TestStringType : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestStringType<CHAR> );
  
	CPPUNIT_TEST( testIterator );
	CPPUNIT_TEST( testIteratorCase );
	CPPUNIT_TEST( testIteratorCompare );
	CPPUNIT_TEST( testUTF8Iterator );

	CPPUNIT_TEST_SUITE_END();

protected:

	typedef std::basic_string<CHAR> tString;

public:

	void setUp() {
		setlocale(LC_ALL, "polish");
		std::locale::global(std::locale("polish"));
	}
	void tearDown() {
	}

	tString lowerString() {
		static std::wstring s(L"abc¹æê123");
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tString upperString() {
		static std::wstring s(L"abc¥ÆÊ123");
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tString testString(std::wstring s) {
		return keepChar<std::basic_string<CHAR> >(s);
	}

protected:


	void testIterator() {
		tString test = lowerString();
		StringType<CHAR, cpACP>::ConstIterator it(test.c_str());
		CPPUNIT_ASSERT( it == test.c_str() );
		CPPUNIT_ASSERT( it.getPosition() == 0 );

		for (unsigned i = 0; i < test.size(); ++i) {
			++it;
		}

		CPPUNIT_ASSERT( it.getPosition() == test.size() );
		CPPUNIT_ASSERT( it.getFoundPosition(test.c_str() + test.size()) == StringType<char>::notFound );

	}

	void testIteratorCase() {
		tString upper = testString(L"¥ÊŒÆAB12");
		tString lower = testString(L"¹êœæab12");
		StringType<CHAR, cpACP>::ConstIterator u(upper.c_str());
		StringType<CHAR, cpACP>::ConstIterator l(lower.c_str());

		for (; u < upper.c_str() + upper.size(); ++u, ++l) {
			CPPUNIT_ASSERT( u.getLower() == l.character() );
			CPPUNIT_ASSERT( l.getUpper() == u.character() );
			CPPUNIT_ASSERT( u.getLower() == l.getLower() );
			CPPUNIT_ASSERT( u.getUpper() == l.getUpper() );
		}
	}

	void testIteratorCompare() {
		tString test1 = testString(L"¥");
		tString test2 = testString(L"¹");
		tString test3 = testString(L"B");
		tString test4 = testString(L"b");

		StringType<CHAR, cpACP>::ConstIterator a;
		StringType<CHAR, cpACP>::ConstIterator b;

		a = test1.c_str();
		b = test2.c_str();
		CPPUNIT_ASSERT( a.charEq(b, false) == false );
		CPPUNIT_ASSERT( a.charEq(b, true) == true );
		CPPUNIT_ASSERT( a.charCmp(b, false) != 0 );
		CPPUNIT_ASSERT( a.charCmp(b, true) == 0 );

		a = test3.c_str();
		b = test4.c_str();
		CPPUNIT_ASSERT( a.charEq(b, false) == false );
		CPPUNIT_ASSERT( a.charEq(b, true) == true );
		CPPUNIT_ASSERT( a.charCmp(b, false) != 0 );
		CPPUNIT_ASSERT( a.charCmp(b, true) == 0 );

		a = test1.c_str();
		b = test3.c_str();
		CPPUNIT_ASSERT( a.charEq(b, false) == false );
		CPPUNIT_ASSERT( a.charEq(b, true) == false );
		CPPUNIT_ASSERT_EQUAL( (signed)-1, a.charCmp(b, false) );
		CPPUNIT_ASSERT( a.charCmp(b, true) == -1 );
		CPPUNIT_ASSERT( b.charCmp(a, false) == 1 );

	}


	void testUTF8Iterator() {
		std::string test = fromUnicode(L"£ódŸ", CP_UTF8);
		StringType<char, cpUTF8>::ConstIterator it(test.c_str());
		CPPUNIT_ASSERT( it == test.c_str() );
		CPPUNIT_ASSERT( it.getPosition() == 0 );

		for (; it < test.c_str() + test.size(); ++it) {
		}

		CPPUNIT_ASSERT( it.getPosition() == 4 );
		CPPUNIT_ASSERT( it - test.c_str() == 7 );

		it -= 4; // £
		it += 2; // d
		CPPUNIT_ASSERT( it.getPosition() == 2 );
		CPPUNIT_ASSERT_EQUAL( 'd', *it );
	}


};


CPPUNIT_TEST_SUITE_REGISTRATION( TestStringType<char> );
CPPUNIT_TEST_SUITE_REGISTRATION( TestStringType<wchar_t> );
