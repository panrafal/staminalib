#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <Stamina/VersionControl.h>
#include <Stamina/CriticalSection.h>
#include <Stamina/Helpers.h>
#include <Stamina/Lock.h>
#include <Stamina/Mutex.h>

#define TEST_STRING

#include "..\String.h"

#include <ConvertUTF.h>

using namespace Stamina;

__declspec(noinline) int testCall(int a) {
	return max(a, 0);
}

class CharType {
public:
	typedef char This;
	typedef wchar_t Other;
};
class WCharType {
public:
	typedef wchar_t This;
	typedef char Other;
};

template <class CHARTYPE>
class TestString : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestString );
  
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testType );
	CPPUNIT_TEST( testMBLock );
	CPPUNIT_TEST( testRef );
//	CPPUNIT_TEST( testAutoRef );
	CPPUNIT_TEST( testCharPos );
//	CPPUNIT_TEST( testCompare );
//	CPPUNIT_TEST( testEqual );
//	CPPUNIT_TEST( testFind );
//	CPPUNIT_TEST( testSubstr );
//	CPPUNIT_TEST( testAssign );
//	CPPUNIT_TEST( testAppend );
//	CPPUNIT_TEST( testPrepend );
//	CPPUNIT_TEST( testInsert );
//	CPPUNIT_TEST( testErase );
//	CPPUNIT_TEST( testReplace );
//	CPPUNIT_TEST( testReplaceChars );
//	CPPUNIT_TEST( testUseBuffer );


	CPPUNIT_TEST_SUITE_END();

protected:

public:

	void setUp() {

	}
	void tearDown() {
	}

protected:

	typedef typename CHARTYPE::This CHAR;
	typedef typename CHARTYPE::Other OTHER;

	typedef std::basic_string<CHAR> tString;
	typedef std::basic_string<OTHER> tOther;

	tString testString(std::wstring s = L"abcDEF¹êæGHI") {
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tOther otherString(std::wstring s = L"abcDEF¹êæGHI") {
		return keepChar<std::basic_string<OTHER> >(s);
	}

	bool isWide() {
		return sizeof(CHAR) == sizeof(wchar_t);
	}

public:

	void testConstructor() {
		{
		String s;
		CPPUNIT_ASSERT( s.empty() == true );
		CPPUNIT_ASSERT( s.isWide() == false );
		CPPUNIT_ASSERT( s.getLength() == 0 );
		}
		{
		String s (testString());
		CPPUNIT_ASSERT( s.empty() == false );
		CPPUNIT_ASSERT( s.isWide() == isWide() );
		CPPUNIT_ASSERT( s.getLength() == testString().size() );
		}
		{
		StringUTF s (fromUnicode( L"¹êæ", CP_UTF8));
		CPPUNIT_ASSERT( s.empty() == false );
		CPPUNIT_ASSERT( s.isWide() == false );
		CPPUNIT_ASSERT( s.getLength() == 3 );
		}
	}

	void testType() {
		{ // szybka konwersja
			String s(testString(L"¹êæ"));
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
		}
		{ // forceType
			String s(testString(L"¹êæ"));
			s.forceType(!isWide());
			CPPUNIT_ASSERT( s.isWide() == !isWide() );
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
		}
		{ // match
			String s(testString(L"¹êæ"));
			s.matchTypes("A"); // nic sie nie powinno zmienic...
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			s.matchTypes(L"W"); // zawsze na wide
			CPPUNIT_ASSERT( s.isWide() == true );
			s.matchTypes("A"); // nic sie nie powinno zmienic...
			CPPUNIT_ASSERT( s.isWide() == true );
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
		}
		{ // UTF8
			StringUTF s (fromUnicode( L"¹êæ", CP_UTF8));
			CPPUNIT_ASSERT( s.isWide() == false );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
		}

	}

	void testMBLock() {
		{ // forceType
			String s;
			s.setMBLock(true);
			s.forceType(true);
			s.assign(testString(L"¹êæ"));
			CPPUNIT_ASSERT( s.isWide() == false );
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
		}
		{ // match
			String s;
			s.setMBLock(true);
			s.assign(testString(L"¹êæ"));
			s.matchTypes("A");
			CPPUNIT_ASSERT( s.isWide() == false );
			s.matchTypes(L"W");
			CPPUNIT_ASSERT( s.isWide() == false );
			s.matchTypes("A");
			CPPUNIT_ASSERT( s.isWide() == false );
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
		}
	}

	void testRef() {
		{ // szybka konwersja
			tString test = testString(L"¹êæ");
			StringRef s(test);
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			CPPUNIT_ASSERT( s.getData<CHAR>() == test.c_str() );
			CPPUNIT_ASSERT( s.getData() == test.c_str() ); // active
			s.makeUnique();
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
			CPPUNIT_ASSERT( s.getData<CHAR>() != test.c_str() );
			CPPUNIT_ASSERT( s.getData() != test.c_str() ); // active
			CPPUNIT_ASSERT( s.isWide() == isWide() );
		}

	}

	void testAutoRef() {
	}

	void testCharPos() {
		{ // UTF8
			StringUTF s (fromUnicode( L"¹êæ", CP_UTF8));
			CPPUNIT_ASSERT( s.isWide() == false );
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, s.getDataPos(1) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)1, s.getCharPos(2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)0, s.getCharPos(1) );
			s.forceType<wchar_t>();
			CPPUNIT_ASSERT( s.isWide() == true );
			CPPUNIT_ASSERT_EQUAL( (unsigned)1, s.getDataPos(1) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, s.getCharPos(2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)0, s.getCharPos(0) );
		}
	}

	void testCompare() {
	}

	void testEqual() {
	}

	void testFind() {
	}

	void testSubstr() {
	}

	void testAssign() {
	}

	void testAppend() {
	}

	void testPrepend() {
	}

	void testInsert() {
	}

	void testErase() {
	}

	void testReplace() {
	}

	void testReplaceChars() {
	}

	void testUseBuffer() {
	}



};

CPPUNIT_TEST_SUITE_REGISTRATION( TestString<CharType> );

