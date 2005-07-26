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
	CPPUNIT_TEST( testTypeLock );
	CPPUNIT_TEST( testRef );
//	CPPUNIT_TEST( testAutoRef );
	CPPUNIT_TEST( testCharPos );
	CPPUNIT_TEST( testEqual );
	CPPUNIT_TEST( testCompare );
	CPPUNIT_TEST( testFind );
	CPPUNIT_TEST( testSubstr );
//	CPPUNIT_TEST( testAssign );
//	CPPUNIT_TEST( testAppend );
//	CPPUNIT_TEST( testPrepend );
//	CPPUNIT_TEST( testInsert );
//	CPPUNIT_TEST( testErase );
//	CPPUNIT_TEST( testReplace );
//	CPPUNIT_TEST( testReplaceChars );
//	CPPUNIT_TEST( testUseBuffer );
//	CPPUNIT_TEST( testPassRef );


	CPPUNIT_TEST_SUITE_END();

protected:

public:

	void setUp() {
		setlocale(LC_ALL, "polish");
		std::locale::global(std::locale("polish"));
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

	void testTypeLock() {
		{ // forceType
			String s;
			s.forceType(isWide()); // ustawiamy
			s.setTypeLock(true); // blokujemy
			s.forceType(!isWide());
			s.assign(otherString(L"¹êæ"));
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
		}
		{ // match
			String s;
			s.forceType(isWide());
			s.setTypeLock(true);
			s.assign(otherString(L"¹êæ"));
			s.matchTypes("A");
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			s.matchTypes(L"W");
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			s.matchTypes("A");
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
		}
	}

	void testRef() {
		{ // szybka konwersja
			String test(testString(L"¹êæ"));
			StringRef s(test);
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			CPPUNIT_ASSERT( s.getData<CHAR>() == test.getData<CHAR>() );
			CPPUNIT_ASSERT( s.getData() == test.getData() ); // active
			s.makeUnique();
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
			CPPUNIT_ASSERT( s.getData<CHAR>() != test.getData<CHAR>() );
			CPPUNIT_ASSERT( s.getData() != test.getData() ); // active
			CPPUNIT_ASSERT( s.isWide() == isWide() );
		}
		{ // wykorzystanie konwersji
			String test(testString(L"¹êæ"));
			test.prepareType<char>();
			test.prepareType<wchar_t>();
			StringRef s(test);
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			CPPUNIT_ASSERT( s.getData<char>() == test.getData<char>() );
			CPPUNIT_ASSERT( s.getData<wchar_t>() == test.getData<wchar_t>() );
			s.makeUnique();
			CPPUNIT_ASSERT_EQUAL( std::string("¹êæ"), std::string(s.a_str()) );
			CPPUNIT_ASSERT_EQUAL( std::wstring(L"¹êæ"), std::wstring(s.w_str()) );
			CPPUNIT_ASSERT( s.getData<char>() != test.getData<char>() );
			CPPUNIT_ASSERT( s.getData<wchar_t>() != test.getData<wchar_t>() );
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

	void testEqual() {
		{
			String a (testString(L"abcd¹êæ"));
			String b (testString(L"ABCD¥ÊÆ"));
			String c (otherString(L"ABCD¥ÊÆ"));
			CPPUNIT_ASSERT( a.equal(b) == false );
			CPPUNIT_ASSERT( a.equal(b, true) == true );
			CPPUNIT_ASSERT( a.equal(c) == false );
			CPPUNIT_ASSERT( a.equal(c, true) == true );
			CPPUNIT_ASSERT( b.equal(c) == true );
			CPPUNIT_ASSERT( b.equal(c, true) == true );

			CPPUNIT_ASSERT( a.equal("ABCd¹êæ", true) == true );
			CPPUNIT_ASSERT( a.equal(L"ABCd¹êæ", true) == true );

			CPPUNIT_ASSERT( a != b );
			CPPUNIT_ASSERT( c == c );
			CPPUNIT_ASSERT( a == "abcd¹êæ" );
			CPPUNIT_ASSERT( a != "abc" );

			CPPUNIT_ASSERT( a.isWide() == isWide() );
			CPPUNIT_ASSERT( b.isWide() == isWide() );
			CPPUNIT_ASSERT( c.isWide() == !isWide() );
		}
	}

	void testCompare() {
		{
			String a (testString(L"a¹cd"));
			String b (testString(L"abcd"));
			String c (otherString(L"A¥CD"));
			String d (otherString(L"ABCD"));
			CPPUNIT_ASSERT_EQUAL( (int)-1, a.compare(b) );
			CPPUNIT_ASSERT_EQUAL( (int)-1, a.compare(d, true) );
			CPPUNIT_ASSERT_EQUAL( (int)1, b.compare(c, true) );
			CPPUNIT_ASSERT_EQUAL( (int)0, a.compare(c, true) );
			CPPUNIT_ASSERT_EQUAL( (int)0, b.compare(d, true) );

			CPPUNIT_ASSERT_EQUAL( (int)-1, a.compare("æææ") );
			CPPUNIT_ASSERT_EQUAL( (int)-1, a.compare(L"æææ") );

			CPPUNIT_ASSERT( a < b );
			CPPUNIT_ASSERT( b > a );
			CPPUNIT_ASSERT( d > b );
			CPPUNIT_ASSERT( a <= a  );
			CPPUNIT_ASSERT( a >= a  );
			CPPUNIT_ASSERT( a < "zzz" );
			CPPUNIT_ASSERT( a < L"zzz" );

			CPPUNIT_ASSERT( a.isWide() == isWide() );
			CPPUNIT_ASSERT( b.isWide() == isWide() );
			CPPUNIT_ASSERT( c.isWide() == !isWide() );
			CPPUNIT_ASSERT( d.isWide() == !isWide() );

		}
	}

	void testFind() {
		//						0123456789012345678901234567890123456789
		String a ( testString(L"G¿eg¿ó³ka by³a bardzo mi³¹ g¿eg¿ó³k¹ :)"));
		String b ( testString(L"¿eg¿"));
		String c (otherString(L"G¯EG¯Ó£KA by³a bardzo mi³¹ g¿eg¿ó³k¹ :)"));
		String d (otherString(L"¯EG¯"));

		CPPUNIT_ASSERT_EQUAL( (unsigned)1, a.find(b) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)-1, a.find(d) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)1, a.find(d, 0, true) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)1, a.find(b, 1) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)28, a.find(b, 2) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)28, a.find(b, 0, false, 1) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)28, a.findLast(b) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)-1, a.find(b, 0, false, 1, 10) );

		CPPUNIT_ASSERT( c & d );
		CPPUNIT_ASSERT( a & "by³a" );
		CPPUNIT_ASSERT( a & L"by³a" );
	}

	void testSubstr() {
		String a ( testString(L"0123456789") );
		CPPUNIT_ASSERT_EQUAL( testString(L"56789"), tString( a.substr(5).str<CHAR>() ) );
		CPPUNIT_ASSERT_EQUAL( testString(L"56"), tString( a.substr(5, 2).str<CHAR>() ) );
		CPPUNIT_ASSERT_EQUAL( tString(), tString( a.substr(20).str<CHAR>() ) );
		CPPUNIT_ASSERT_EQUAL( tString(), tString( a.substr(0, 0).str<CHAR>() ) );
		CPPUNIT_ASSERT_EQUAL( testString(L"0123456789"), tString( a.substr(0, 20).str<CHAR>() ) );
		CPPUNIT_ASSERT_EQUAL( otherString(L"56"), tOther( a.substr(5, 2).str<OTHER>() ) );
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
//CPPUNIT_TEST_SUITE_REGISTRATION( TestString<WCharType> );

