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
#include "..\WideChar.h"

#include <ConvertUTF.h>

using namespace Stamina;

__declspec(noinline) int testCall(int a) {
	return max(a, 0);
}

class CharType {
public:
	typedef stACP Type;
	typedef stACP Encoding;
	typedef char This;
	typedef wchar_t Other;
};
class WCharType {
public:
	typedef stUNICODE Type;
	typedef stACP Encoding;
	typedef wchar_t This;
	typedef char Other;
};
class UTF8Type {
public:
	typedef stUTF8 Type;
	typedef stUTF8 Encoding;
	typedef char This;
	typedef wchar_t Other;
};

const wchar_t * const strLong = L"012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
const wchar_t * const strMed = L"abcdefghijklmnopqrstuwxyz";


template <class CHARTYPE>
class TestString : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestString );
  
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testType );
	CPPUNIT_TEST( testTypeLock );
	CPPUNIT_TEST( testRef );
	CPPUNIT_TEST( testCharPos );
	CPPUNIT_TEST( testLength );
	CPPUNIT_TEST( testSinglebyte );
	CPPUNIT_TEST( testEqual );
	CPPUNIT_TEST( testCompare );
	CPPUNIT_TEST( testFind );
	CPPUNIT_TEST( testFindChars );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testAppend );
	CPPUNIT_TEST( testPassRef );
	CPPUNIT_TEST( testSubstr );
	CPPUNIT_TEST( testPrepend );
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testErase );
	CPPUNIT_TEST( testReplace );
	CPPUNIT_TEST( testReplaceChars );
	CPPUNIT_TEST( testMakeLower );
	CPPUNIT_TEST( testMakeUpper );
	CPPUNIT_TEST( testUseBuffer );
	CPPUNIT_TEST( testTyped );
	CPPUNIT_TEST( testConversion );

	/*TODO: zrobic testy ze StringRef!!!*/

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
	typedef typename CHARTYPE::Type TYPE;
	typedef StringT< typename CHARTYPE::Encoding > String;
	typedef StringRefT< typename CHARTYPE::Encoding > StringRef;
	typedef typename StringRef::PassStringRef PassStringRef;

	typedef std::basic_string<CHAR> tString;
	typedef std::basic_string<OTHER> tOther;

	tString testString(std::wstring s) {
		return keepChar<std::basic_string<CHAR> >(s, TYPE::codepage);
	}

	tOther otherString(std::wstring s) {
		return keepChar<std::basic_string<OTHER> >(s, TYPE::codepage);
	}

	tString testString(std::string s) {
		return keepChar<std::basic_string<CHAR> >(s, TYPE::codepage);
	}

	tOther otherString(std::string s) {
		return keepChar<std::basic_string<OTHER> >(s, TYPE::codepage);
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
		String s (testString(L"¹bæ"));
		CPPUNIT_ASSERT( s.empty() == false );
		CPPUNIT_ASSERT( s.isWide() == isWide() );
		CPPUNIT_ASSERT( s.getLength() == 3 );
		}
		{
		String s (testString(L"¹bæ"));

		const String a = s;
		CPPUNIT_ASSERT( s.str<CHAR>() != a.str<CHAR>() );
		
		String b;
		b = s;
		CPPUNIT_ASSERT( s.str<CHAR>() != b.str<CHAR>() );

		}
	}

	void testType() {
		{ // szybka konwersja
			String s(testString(L"¹êæ"));
			CPPUNIT_ASSERT_EQUAL( testString(L"¹êæ"), testString(s.str<CHAR>()) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"¹êæ"), otherString(s.str<OTHER>()) );
		}
		{ // forceType
			String s(testString(L"¹êæ"));
			s.forceType(!isWide());
			CPPUNIT_ASSERT( s.isWide() == !isWide() );
			CPPUNIT_ASSERT_EQUAL( testString(L"¹êæ"), testString(s.str<CHAR>()) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"¹êæ"), otherString(s.str<OTHER>()) );
		}
		{ // match
			String s(testString(L"¹êæ"));
			s.matchTypes("A"); // nic sie nie powinno zmienic...
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			s.matchTypes(L"W"); // zawsze na wide
			CPPUNIT_ASSERT( s.isWide() == true );
			s.matchTypes("A"); // nic sie nie powinno zmienic...
			CPPUNIT_ASSERT( s.isWide() == true );
			CPPUNIT_ASSERT_EQUAL( testString(L"¹êæ"), testString(s.str<CHAR>()) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"¹êæ"), otherString(s.str<OTHER>()) );
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
			CPPUNIT_ASSERT_EQUAL( testString(L"¹êæ"), testString(s.str<CHAR>()) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"¹êæ"), otherString(s.str<OTHER>()) );
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
			CPPUNIT_ASSERT_EQUAL( testString(L"¹êæ"), testString(s.str<CHAR>()) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"¹êæ"), otherString(s.str<OTHER>()) );
		}
	}

	void testRef() {
		{ // szybka konwersja
			String test(testString(L"¹êæ"));
			StringRef s(test);
			CPPUNIT_ASSERT_EQUAL( testString(L"¹êæ"), testString(s.str<CHAR>()) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"¹êæ"), otherString(s.str<OTHER>()) );
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			CPPUNIT_ASSERT( s.getData<CHAR>() == test.getData<CHAR>() );
			CPPUNIT_ASSERT( s.getData() == test.getData() ); // active
			s.makeUnique();
			CPPUNIT_ASSERT_EQUAL( testString(L"¹êæ"), testString(s.str<CHAR>()) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"¹êæ"), otherString(s.str<OTHER>()) );
			CPPUNIT_ASSERT( s.getData<CHAR>() != test.getData<CHAR>() );
			CPPUNIT_ASSERT( s.getData() != test.getData() ); // active
			CPPUNIT_ASSERT( s.isWide() == isWide() );
		}
		{ // wykorzystanie konwersji
			String test(testString(L"¹êæ"));
			test.prepareType<char>();
			test.prepareType<wchar_t>();
			StringRef s(test);
			CPPUNIT_ASSERT_EQUAL( testString(L"¹êæ"), testString(s.str<CHAR>()) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"¹êæ"), otherString(s.str<OTHER>()) );
			CPPUNIT_ASSERT( s.isWide() == isWide() );
			CPPUNIT_ASSERT( s.getData<char>() == test.getData<char>() );
			CPPUNIT_ASSERT( s.getData<wchar_t>() == test.getData<wchar_t>() );
			s.makeUnique();
			CPPUNIT_ASSERT_EQUAL( testString(L"¹êæ"), testString(s.str<CHAR>()) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"¹êæ"), otherString(s.str<OTHER>()) );
			CPPUNIT_ASSERT( s.getData<char>() != test.getData<char>() );
			CPPUNIT_ASSERT( s.getData<wchar_t>() != test.getData<wchar_t>() );
			CPPUNIT_ASSERT( s.isWide() == isWide() );
		}
		{
			String a ("Hello");
			a.assignCheapReference(a);
			CPPUNIT_ASSERT_EQUAL( String("Hello"), a );
		}

	}

	PassStringRef testPassRef1(const StringRef& a, const StringRef& b) {
		return a + b;
	}
	String testPassRef2(StringRef a, int cut) {
		if (cut) {
			return PassStringRef(a.substr(cut, 1));
		} else {
			return PassStringRef(StringRef("Cut!"));
		}
	}

	void testPassRef() {
		{
			String a = "AA";
			String b = "BB";
			a = PassStringRef(b);
			CPPUNIT_ASSERT( a == "BB" );
			CPPUNIT_ASSERT( b.empty() == true );
		}
		{
			char* c = "Hello";
			String a = PassStringRef(StringRef(c));
			CPPUNIT_ASSERT( a == c );
			CPPUNIT_ASSERT( a.a_str() == c );
		}
		{
			String a = "Hello";
			String cpy = "Hello";
			CPPUNIT_ASSERT_EQUAL( String("Cut!") , testPassRef2(a, 0) );
			CPPUNIT_ASSERT_EQUAL( String("e") , testPassRef2(a, 1) );
			CPPUNIT_ASSERT_EQUAL( String("Cut!") , testPassRef2(a, 0) );
			CPPUNIT_ASSERT_EQUAL( cpy , a );
		}
		{
			String a = "Hello";
			String b = " world!";
			String c = testPassRef1(a, b);
			CPPUNIT_ASSERT_EQUAL( String("Hello world!") , c );
			CPPUNIT_ASSERT_EQUAL( String("Hello world!") , String(testPassRef1(a, b)) );
			CPPUNIT_ASSERT_EQUAL( String("Hello world!") , String(testPassRef1(a, b)) );
		}
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

	void testLength() {
		{ // UTF8
			StringUTF s (fromUnicode( L"¹êæ", CP_UTF8));
			CPPUNIT_ASSERT_EQUAL( (unsigned)3, s.getLength() );
			CPPUNIT_ASSERT_EQUAL( (unsigned)3, s.getLength() );
			s.forceType<wchar_t>();
			CPPUNIT_ASSERT_EQUAL( (unsigned)3, s.getLength() );
		}
	}

	void testSinglebyte() {
		{ // UTF8
			StringUTF s (fromUnicode( L"¹êæ", CP_UTF8));
			CPPUNIT_ASSERT( s.isSinglebyte<char>() == false );
			CPPUNIT_ASSERT_EQUAL( (unsigned)3, s.getLength() );
			CPPUNIT_ASSERT( s.isSinglebyte<char>() == false );
			s.forceType<wchar_t>();
			s = L"¹êæ";
			CPPUNIT_ASSERT( s.isSinglebyte<wchar_t>() == true );
			CPPUNIT_ASSERT_EQUAL( (unsigned)3, s.getLength() );
			CPPUNIT_ASSERT( s.isSinglebyte<wchar_t>() == true );
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

			CPPUNIT_ASSERT( a.equal(testString(L"ABCd¹êæ"), true) == true );
			CPPUNIT_ASSERT( a.equal(otherString(L"ABCd¹êæ"), true) == true );

			CPPUNIT_ASSERT( a != b );
			CPPUNIT_ASSERT( c == c );
			CPPUNIT_ASSERT( a == testString(L"abcd¹êæ") );
			CPPUNIT_ASSERT( a != testString(L"abc") );
			CPPUNIT_ASSERT( a == otherString(L"abcd¹êæ") );
			CPPUNIT_ASSERT( a != otherString(L"abc") );

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
		CPPUNIT_ASSERT_EQUAL( (unsigned)28, a.find(b, 27, false, 0, 6) );

		CPPUNIT_ASSERT( c & d );
		CPPUNIT_ASSERT( a & testString(L"by³a") );
		CPPUNIT_ASSERT( a & otherString(L"by³a") );
	}

	void testFindChars() {
		//						0123456789012345678901234567890123456789
		String a ( testString(L"g¿eg¿ó³ka by³a bardzo mi³¹ g¿eg¿ó³k¹ :)"));
		String b ( testString(L"¿eg"));
		String c (otherString(L"G¯EG¯Ó£KA by³a bardzo mi³¹ g¿eg¿ó³k¹ :)"));
		String d (otherString(L"¯EG"));

		CPPUNIT_ASSERT_EQUAL( (unsigned)0, a.findChars(b) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)-1, a.findChars(d) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)0, a.findChars(d, 0, true) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)1, a.findChars(b, 1) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)27, a.findChars(b, 10) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)4, a.findChars(b, 0, false, 4) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)31, a.findLastChars(b) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)-1, a.findChars(b, 0, false, 6, 10) );

		CPPUNIT_ASSERT( c ^ d );
		CPPUNIT_ASSERT( a ^ testString(L"ó") );
		CPPUNIT_ASSERT( a ^ otherString(L"¹") );
	}


	void testSubstr() {
		String a ( testString(L"0123456789") );
		String cpy = a;
		{
			CPPUNIT_ASSERT_EQUAL( testString(L"56789"), tString( a.substr(5).str<CHAR>() ) );
			CPPUNIT_ASSERT_EQUAL( testString(L"56"), tString( a.substr(5, 2).str<CHAR>() ) );
			CPPUNIT_ASSERT_EQUAL( tString(), tString( a.substr(20).str<CHAR>() ) );
			CPPUNIT_ASSERT_EQUAL( tString(), tString( a.substr(0, 0).str<CHAR>() ) );
			CPPUNIT_ASSERT_EQUAL( testString(L"0123456789"), tString( a.substr(0, 20).str<CHAR>() ) );
			CPPUNIT_ASSERT_EQUAL( otherString(L"56"), tOther( a.substr(5, 2).str<OTHER>() ) );
			CPPUNIT_ASSERT_EQUAL( cpy, a );
		}
		{
			StringRef b ( a );
			CPPUNIT_ASSERT_EQUAL( testString(L"56789"), tString( b.substr(5).str<CHAR>() ) );
			CPPUNIT_ASSERT_EQUAL( StringRef(cpy), b );
		}
	}

	void testAssign() {
		{ // kasowanie konwersji
			String a ( testString(L"Blah") );
			a.prepareType<OTHER>();
			CPPUNIT_ASSERT( a.isWide() == isWide() );
			a = otherString(L"Hello");
			CPPUNIT_ASSERT( a.isWide() != isWide() );
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) == "" );
			a.prepareType<CHAR>();
			a = StringRef( otherString(L"Blah") );
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) == "" );
			String b ( testString(L"Hello") );
			a = b;
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) == "" );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			String a( testString(L"Blah") );
			a = StringRef(test.c_str());
			CPPUNIT_ASSERT( a.getData<CHAR>() != test.c_str() );
			CPPUNIT_ASSERT_EQUAL( String(test), a );
			a.clear();
			a = test;
			CPPUNIT_ASSERT( a.getData<CHAR>() != test.c_str() );
			CPPUNIT_ASSERT_EQUAL( String(test), a );
		}
		{ // przypisania
			String a;
			a = testString(L"A");
			CPPUNIT_ASSERT_EQUAL( String("A"), a );
			a = otherString(L"B");
			CPPUNIT_ASSERT_EQUAL( String("B"), a );
			a = "C";
			CPPUNIT_ASSERT_EQUAL( String("C"), a );
			a = L"D";
			CPPUNIT_ASSERT_EQUAL( String("D"), a );
		}
		{ // przypisania do StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			a = testString(L"Aaa");
			CPPUNIT_ASSERT( a.str<CHAR>() != test.c_str() );
		}
		{ // podwójne przypisanie
			String a = testString("Hello");
            a = a;
			CPPUNIT_ASSERT_EQUAL(String("Hello"), a);
		}
	}

	void testAppend() {
		{
			String a (testString(L"Hello "));
			String b (otherString(L"world!"));
			a.prepareType<OTHER>();
			a.append(b);
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "Hello " );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "Hello " );
			CPPUNIT_ASSERT_EQUAL( String("Hello world!"), a );
		}
		{
			String a ("Hello ");
			a += L"world!";
			CPPUNIT_ASSERT_EQUAL( String("Hello world!"), a );
		}
		{
			String a ("Hello ");
			CPPUNIT_ASSERT_EQUAL( StringRef("Hello world!"), StringRef(a + "world!") );
			CPPUNIT_ASSERT_EQUAL( String("Hello "), a );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			a.append( testString(L"Aaa") );
			CPPUNIT_ASSERT( a.str<CHAR>() != test.c_str() );
		}
		{
			String a (testString(strLong));
			String _a = a;
			String b (testString(strMed));
			String _b = b;
			String c = a + b;
			CPPUNIT_ASSERT_EQUAL( String(_a + _b), c );
			b = b + a;
			CPPUNIT_ASSERT_EQUAL( String(_b + _a), b );
			b = _b;
			a = b + a;
			CPPUNIT_ASSERT_EQUAL( String(_b + _a), a );
			a = _a;
			a = "aaaa" + a;
			CPPUNIT_ASSERT_EQUAL( String("aaaa" + std::string(_a.a_str())), a  );
			a = _a;
			a.append(a);
			CPPUNIT_ASSERT_EQUAL( String(_a + _a), a );

		}
	}

	void testPrepend() {
		{
			String a (testString(L"world!"));
			String b (otherString(L"Hello "));
			a.prepareType<OTHER>();
			a.prepend(b);
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "world!" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "world!" );
			CPPUNIT_ASSERT_EQUAL( String("Hello world!"), a );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			a.prepend( testString(L"Aaa") );
			CPPUNIT_ASSERT( a.str<CHAR>() != test.c_str() );
		}
		{
			String a (testString(strLong));
			String _a = a;
			String b (testString(strMed));
			String _b = b;
			b.prepend(b);
			CPPUNIT_ASSERT_EQUAL( String(_b + _b), b );
			a.prepend(a);
			CPPUNIT_ASSERT_EQUAL( String(_a + _a), a );
		}
	}

	void testInsert() {
		{
			String a (testString(L"AbbA"));
			String b (otherString(L"li"));
			a.prepareType<OTHER>();
			a.insert(2, b);
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "AbbA" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "AbbA" );
			CPPUNIT_ASSERT_EQUAL( String("AblibA"), a );
		}
		{
			String a (testString(L"AbbA"));
			String b (otherString(L"li"));
			a.insert(0, b);
			CPPUNIT_ASSERT_EQUAL( String("liAbbA"), a );
		}
		{
			String a (testString(L"AbbA"));
			String b (otherString(L"li"));
			a.insert(50, b);
			CPPUNIT_ASSERT_EQUAL( String("AbbAli"), a );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			a.insert( 2, testString(L"Aaa") );
			CPPUNIT_ASSERT( a.str<CHAR>() != test.c_str() );
		}
		{
			tString _a = testString(strLong);
			String a (_a);
			a.insert(10, a);
			CPPUNIT_ASSERT_EQUAL( String( _a.substr(0, 10) + _a + _a.substr(10) ) , a );
		}

	}

	void testErase() {
		{
			String a (testString(L"0123456"));
			a.prepareType<OTHER>();
			a.erase(2, 4);
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "0123456" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "0123456" );
			CPPUNIT_ASSERT_EQUAL( String("016"), a );
		}
		{
			String a (testString(L"0123456"));
			a.erase(0, 2);
			CPPUNIT_ASSERT_EQUAL( String("23456"), a );
		}
		{
			String a (testString(L"0123456"));
			a.erase(20, 2);
			CPPUNIT_ASSERT_EQUAL( String("0123456"), a );
		}
		{
			String a (testString(L"0123456"));
			a.erase(4, 2);
			CPPUNIT_ASSERT_EQUAL( String("01236"), a );
		}
		{
			String a (testString(L"0123456"));
			a.erase(0, 200);
			CPPUNIT_ASSERT_EQUAL( String(""), a );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			a.erase( 1, 3 );
			CPPUNIT_ASSERT( a.str<CHAR>() != test.c_str() );
		}

	}

	void testReplace() {
		{
			String a (testString(L"0123456"));
			a.prepareType<OTHER>();
			a.replace(3, "abc", 0);
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "0123456" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "0123456" );
			CPPUNIT_ASSERT_EQUAL( String("012abc3456"), a );
			a.replace(3, "ABC", 3);
			CPPUNIT_ASSERT_EQUAL( String("012ABC3456"), a );
			a.replace(3, "xyz");
			CPPUNIT_ASSERT_EQUAL( String("012xyz"), a );
			a.replace(0, "XYZ");
			CPPUNIT_ASSERT_EQUAL( String("XYZ"), a );
		}
		{
			String a (testString(L"012__5_7_"));
			a.prepareType<OTHER>();
			a.replace("_", "abc");
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "012__5_7_" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "012__5_7_" );
			CPPUNIT_ASSERT_EQUAL( String("012abcabc5abc7abc"), a );
			a.replace("abc", "ABC", 9, false, 0, -1, 6);
			CPPUNIT_ASSERT_EQUAL( String("012abcabc5ABC7abc"), a );
			a.replace("abc", "ABC", 9, false, 0, -1, 8);
			CPPUNIT_ASSERT_EQUAL( String("012abcabc5ABC7ABC"), a );
			a.replace("abc", "xyz", 0, true);
			CPPUNIT_ASSERT_EQUAL( String("012xyzxyz5xyz7xyz"), a );
		}
		{
			String a (testString(L"abc123abc123abc123"));
			a.replace("abc", "");
			CPPUNIT_ASSERT_EQUAL( String("123123123"), a );
		}
		{
			String a (testString(L"abc123abc123abc123"));
			a.replace("abc", "abcdef");
			CPPUNIT_ASSERT_EQUAL( String("abcdef123abcdef123abcdef123"), a );
		}
		{
			String a (testString(L"abc123abc123abc123"));
			a.replace("abc", "A", 3, false, 1, String::lengthUnknown, 15);
			CPPUNIT_ASSERT_EQUAL( String("abc123abc123A123"), a );
		}
		{
			String a (testString(L"abc123abc123abc123"));
			a.replace("abc", "A", 0, false, -1, String::lengthUnknown, 9);
			CPPUNIT_ASSERT_EQUAL( String("abc123A123abc123"), a );
		}
		{
			String a (testString(L"abc123abc123abc123"));
			a.replace("abc", "A", 0, false, 0, 2);
			CPPUNIT_ASSERT_EQUAL( String("A123A123abc123"), a );
		}
		{
			String a (testString(L"abc123abc123abc123"));
			a.replace("abc", "A", 0, false, 0, -1, 13);
			CPPUNIT_ASSERT_EQUAL( String("A123A123abc123"), a );
		}
		{
			String a (testString(L"0123456"));
			a -= "123";
			CPPUNIT_ASSERT_EQUAL( String("0456"), a );
		}
		{
			String a (testString(L"0123444"));
			CPPUNIT_ASSERT_EQUAL( StringRef("0444"), a - "123" );
			CPPUNIT_ASSERT_EQUAL( StringRef("0123444"), a - "123." );
			CPPUNIT_ASSERT_EQUAL( StringRef("0123"), a - "4" );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			a.replace( "l", "L" );
			CPPUNIT_ASSERT( a.str<CHAR>() != test.c_str() );
		}
		{
			String a( testString(strLong) );
			String _a = a;
			a.replace( a, a );
			CPPUNIT_ASSERT_EQUAL( _a, a );

			String b = _a;
			b.replace("01234", _a);

			a = _a;
			a.replace("01234", a);
			CPPUNIT_ASSERT_EQUAL( b, a );

			a = _a;
			a.replace(2, a);
			CPPUNIT_ASSERT_EQUAL( String(_a.substr(0, 2) + _a), a );

		}
	}

	void testReplaceChars() {
		{
			String a (testString(L"abcdefghABCDEFGH"));
			a.prepareType<OTHER>();
			a.replaceChars("ace", "xyz");
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "abcdefghABCDEFGH" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "abcdefghABCDEFGH" );
			CPPUNIT_ASSERT_EQUAL( String("xbydzfghABCDEFGH"), a );
			a.replaceChars("bdf", "xyz", true, true);
			CPPUNIT_ASSERT_EQUAL( String("xxyyzzghAXCYEZGH"), a );
			a.replaceChars("xyz", "", true, false, true, 2);
			CPPUNIT_ASSERT_EQUAL( String("xxyyzzAXCYEZGH"), a );
			a.replaceChars("xyz", "_", true, true, false, 8);
			CPPUNIT_ASSERT_EQUAL( String("______A_C_EZGH"), a );
			a.replaceChars("_", "");
			CPPUNIT_ASSERT_EQUAL( String("ACEZGH"), a );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			a.replaceChars( "l", "L" );
			CPPUNIT_ASSERT( a.str<CHAR>() != test.c_str() );
		}
		{
			String a( testString(strMed) );
			String _a = a;
			a.replaceChars(a, a);
			CPPUNIT_ASSERT_EQUAL(_a, a);

			a = _a;
			a.replaceChars(a, "");
			CPPUNIT_ASSERT_EQUAL(String(), a);
		}

	}

	void testMakeLower() {
		{
			String a (testString(L"abcdefghABCDEFGH"));
			a.prepareType<OTHER>();
			a.makeLower();
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "abcdefghABCDEFGH" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "abcdefghABCDEFGH" );
			CPPUNIT_ASSERT_EQUAL( String("abcdefghabcdefgh"), a );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			a.makeLower();
			CPPUNIT_ASSERT( a.str<CHAR>() != test.c_str() );
		}
		{
			String a (testString(L"abcdefghABCDEFGH"));
			a.prepareType<OTHER>();
			CPPUNIT_ASSERT_EQUAL( StringRef("abcdefghabcdefgh"), a.toLower() );
			CPPUNIT_ASSERT_EQUAL( String("abcdefghABCDEFGH"), a );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			CPPUNIT_ASSERT_EQUAL( StringRef("hello"), a.toLower() );
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
		}
	}

	void testMakeUpper() {
		{
			String a (testString(L"abcdefghABCDEFGH"));
			a.prepareType<OTHER>();
			a.makeUpper();
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "abcdefghABCDEFGH" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "abcdefghABCDEFGH" );
			CPPUNIT_ASSERT_EQUAL( String("ABCDEFGHABCDEFGH"), a );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			a.makeUpper();
			CPPUNIT_ASSERT( a.str<CHAR>() != test.c_str() );
		}
		{
			String a (testString(L"abcdefghABCDEFGH"));
			a.prepareType<OTHER>();
			CPPUNIT_ASSERT_EQUAL( StringRef("ABCDEFGHABCDEFGH"), a.toUpper() );
			CPPUNIT_ASSERT_EQUAL( String("abcdefghABCDEFGH"), a );
		}
		{ // StringRef
			tString test = testString(L"Hello");
			StringRef a ( test ); 
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
			CPPUNIT_ASSERT_EQUAL( StringRef("HELLO"), a.toUpper() );
			CPPUNIT_ASSERT( a.str<CHAR>() == test.c_str() );
		}
	}

	void testUseBuffer() {
		{
			String a;
			CHAR * ch = a.useBuffer<CHAR>(10);
			StringBuffer<CHAR>::copy(ch, testString(L"01234").c_str(), 6);
			a.releaseBuffer<CHAR>();
			CPPUNIT_ASSERT_EQUAL( String("01234"), a );
		}
		{
			String a;
			CHAR * ch = a.useBuffer<CHAR>(10);
			StringBuffer<CHAR>::copy(ch, testString(L"01234").c_str(), 5);
			a.releaseBuffer<CHAR>(5);
			CPPUNIT_ASSERT_EQUAL( String("01234"), a );
		}
		{ // dlugosc rowna buforowi
			String a;
			CHAR * ch = a.useBuffer<CHAR>(10);
			StringBuffer<CHAR>::copy(ch, testString(L"0123456789").c_str(), 11);
			a.releaseBuffer<CHAR>();
			CPPUNIT_ASSERT_EQUAL( String("0123456789"), a );
		}
		{ // stary bufor
			String a (testString(L"abcdefghij"));
			const CHAR * old = a.getData<CHAR>();
			CHAR * ch = a.useBuffer<CHAR>(10);
			StringBuffer<CHAR>::copy(ch, testString(L"0123456789").c_str(), 11);
			a.releaseBuffer<CHAR>();
			CPPUNIT_ASSERT_EQUAL( String("0123456789"), a );
			CPPUNIT_ASSERT( ch == old );
		}
		{ // zmiana typu
			String a (otherString(L"abcdefghijklmno"));
			CHAR * ch = a.useBuffer<CHAR>(10);
			StringBuffer<CHAR>::copy(ch, testString(L"0123456789").c_str(), 11);
			a.releaseBuffer<CHAR>();
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "abcdefghijklmno" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "abcdefghijklmno" );
			CPPUNIT_ASSERT_EQUAL( String("0123456789"), a );
		}
		{ // zmiana typu w zablokowanym
			String a (otherString(L"abcdefghijklmno"));
			a.setTypeLock(true); // blokujemy na OTHER
			CHAR * ch = a.useBuffer<CHAR>(10);
			StringBuffer<CHAR>::copy(ch, testString(L"0123456789").c_str(), 11);
			a.releaseBuffer<CHAR>(); // konwersja do OTHER
			CPPUNIT_ASSERT( StringRef(a.getData<CHAR>()) != "abcdefghijklmno" );
			CPPUNIT_ASSERT( StringRef(a.getData<OTHER>()) != "abcdefghijklmno" );
			CPPUNIT_ASSERT_EQUAL( String("0123456789"), a );
			CPPUNIT_ASSERT( a.isActive<CHAR>() == false );
		}
	}

	void testTyped() {
	}

	void testConversion() {

		String a = testString(L"abc¹êæABC¥ÊÆ");
		StringUTF b = fromUnicode(L"abc¹êæABC¥ÊÆ", CP_UTF8);

		String c = b;
		
		c == b;
		c == StringUTF("abc");

		CPPUNIT_ASSERT( a == b );
		CPPUNIT_ASSERT( a == c );
		CPPUNIT_ASSERT( b == c );

	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestString<CharType> );
CPPUNIT_TEST_SUITE_REGISTRATION( TestString<WCharType> );
CPPUNIT_TEST_SUITE_REGISTRATION( TestString<UTF8Type> );

