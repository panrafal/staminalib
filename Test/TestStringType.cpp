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
	CPPUNIT_TEST( testGetLength );
	CPPUNIT_TEST( testGetDataPos );
	CPPUNIT_TEST( testGetCharPos );
	CPPUNIT_TEST( testEqual );
	CPPUNIT_TEST( testCompare );
	CPPUNIT_TEST( testFind );
	CPPUNIT_TEST( testFindChars );
	CPPUNIT_TEST( testReplaceChars );
	CPPUNIT_TEST( testMakeLower );
	CPPUNIT_TEST( testMakeUpper );

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

		{
			tString toStr = testString(L"¥BC");
			tString fromStr = testString(L"¹bc");
			StringType<CHAR, cpACP>::Iterator to((CHAR*)toStr.c_str());
			StringType<CHAR, cpACP>::ConstIterator from(fromStr.c_str());
			// test przypisania
			to = (CHAR*)from();
			to = (CHAR*)toStr.c_str();
			// podmianki
            to.set(from);
			++to; ++from;
			to.set(from.character());
			++to; ++from;
			*to = *from;
			CPPUNIT_ASSERT_EQUAL(fromStr, toStr);
		}

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

		StringType<CHAR, cpUTF8>::ConstIterator a;
		StringType<CHAR, cpUTF8>::ConstIterator b;

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
		a.charCmp(b, false);
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

		std::string lower = fromUnicode(L"³", CP_UTF8);
		it = test.c_str();
		__int64 character = it.character();
		__int64 buff = it.getLower();
		std::wstring result = toUnicode((char*)&buff, CP_UTF8);
		//CPPUNIT_ASSERT_EQUAL( std::wstring(L"³"), result );
		//CPPUNIT_ASSERT_EQUAL( lower, std::string((char*)&buff) );
		// Zmiana rozmiaru jest zabroniona dla UTF-8
		CPPUNIT_ASSERT_EQUAL( std::wstring(L"£"), result );
		it = "D";
		buff = it.getLower();
		result = toUnicode((char*)&buff, CP_UTF8);
		CPPUNIT_ASSERT_EQUAL( std::wstring(L"d"), result );

		{
			std::string toStr = fromUnicode(L"¥BC", CP_UTF8);
			std::string fromStr = fromUnicode(L"¹bc", CP_UTF8);
			StringType<char, cpUTF8>::Iterator to((char*)toStr.c_str());
			StringType<char, cpUTF8>::ConstIterator from(fromStr.c_str());
			// test przypisania
			to = (char*)from();
			to = (char*)toStr.c_str();
			// podmianki
            to.set(from);
			++to; ++from;
			to.set(from.character());
			++to; ++from;
			*to = *from;
			CPPUNIT_ASSERT_EQUAL(fromStr, toStr);
		}


	}

	void testGetLength() {
		tString test = testString(L"abc¥ÊÆ");
		CPPUNIT_ASSERT_EQUAL( (unsigned)6, StringType<CHAR>::getLength(test.c_str(), test.c_str() + test.size()) );

		typedef StringType<char, cpUTF8> testType;
		std::string utf8 = fromUnicode(L"abc¥ÊÆ", CP_UTF8);
		CPPUNIT_ASSERT_EQUAL( (unsigned)6, testType::getLength(utf8.c_str(), utf8.c_str() + utf8.size()) );
	}

	void testGetDataPos() {
		{
		tString test = testString(L"abc¥ÊÆ");
		const CHAR* from = test.c_str();
		const CHAR* to = from + test.size();

		typedef StringType<CHAR> testType;

		CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getDataPos(from, to, 0) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)4, testType::getDataPos(from, to, 4) );
		// przesadzony
		CPPUNIT_ASSERT_EQUAL( (unsigned)6, testType::getDataPos(from, to, 20) );
		// przesadzony - zero
		CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getDataPos(from, from, 20) );
		// ujemny
		CPPUNIT_ASSERT_EQUAL( (unsigned)5, testType::getDataPos(from, to, -1) );
		// ujemny - zero
		CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getDataPos(from, from, -2) );
		// ujemny przesadzony
		CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getDataPos(from, to, -20) );		
		}
		{ // UTF8
		std::string test = fromUnicode(L"abc¥ÊÆ", CP_UTF8);
		const char* from = test.c_str();
		const char* to = from + test.size();

		typedef StringType<char, cpUTF8> testType;

		CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getDataPos(from, to, 0) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)5, testType::getDataPos(from, to, 4) );
		// przesadzony
		CPPUNIT_ASSERT_EQUAL( (unsigned)9, testType::getDataPos(from, to, 20) );
		// ujemny
		CPPUNIT_ASSERT_EQUAL( (unsigned)7, testType::getDataPos(from, to, -1) );
		// ujemny przesadzony
		CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getDataPos(from, to, -20) );		

		}
	}

	void testGetCharPos() {
		{
		tString test = testString(L"abc¥ÊÆ");
		const CHAR* from = test.c_str();
		const CHAR* to = from + test.size();

		typedef StringType<CHAR> testType;

		CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getCharPos(from, to, 0) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)4, testType::getCharPos(from, to, 4) );
		// przesadzony
		CPPUNIT_ASSERT_EQUAL( (unsigned)6, testType::getCharPos(from, to, 20) );
		}
		{ // UTF8
		std::string test = fromUnicode(L"abc¥ÊÆ", CP_UTF8);
		const char* from = test.c_str();
		const char* to = from + test.size();

		typedef StringType<char, cpUTF8> testType;

		CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getCharPos(from, to, 0) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)4, testType::getCharPos(from, to, 5) );
		CPPUNIT_ASSERT_EQUAL( (unsigned)4, testType::getCharPos(from, to, 6) );
		// przesadzony
		CPPUNIT_ASSERT_EQUAL( (unsigned)6, testType::getCharPos(from, to, 20) );
		}
	}

	void testEqual() {
		{
			tString a = testString(L"abc¥ÊÆ");
			const CHAR* a1 = a.c_str();
			const CHAR* a2 = a1 + a.size();
			tString b = testString(L"ABC¹êæ");
			const CHAR* b1 = b.c_str();
			const CHAR* b2 = b1 + b.size();

			typedef StringType<CHAR> testType;

			CPPUNIT_ASSERT( testType::equal(a1, a2, b1, b2, false) == false );
			CPPUNIT_ASSERT( testType::equal(a1, a2, b1, b2, true) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a2, a1, a2, false) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a2, a1, a2, true) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a1, a1, a1, false) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a1, b1, b1, false) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a2, a1, a2 - 2, true) == false );
		}
		{// UTF8
			std::string a = fromUnicode(L"abc¥ÊÆ", CP_UTF8);
			const char* a1 = a.c_str();
			const char* a2 = a1 + a.size();
			std::string b = fromUnicode(L"ABC¥ÊÆ", CP_UTF8);
			const char* b1 = b.c_str();
			const char* b2 = b1 + b.size();

			typedef StringType<char, cpUTF8> testType;

			CPPUNIT_ASSERT( testType::equal(a1, a2, b1, b2, false) == false );
			CPPUNIT_ASSERT( testType::equal(a1, a2, b1, b2, true) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a2, a1, a2, false) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a2, a1, a2, true) == true );
		}

	}

	void testCompare() {
		{
			tString a = testString(L"a¹¯¥Bc");
			const CHAR* a1 = a.c_str();
			const CHAR* a2 = a1 + a.size();
			tString b = testString(L"abZ¹Bd");
			const CHAR* b1 = b.c_str();
			const CHAR* b2 = b1 + b.size();

			typedef StringType<CHAR> testType;

			CPPUNIT_ASSERT( testType::compare(a1, a2, b1, b2, false) == -1 );
			CPPUNIT_ASSERT( testType::compare(a1+2, a2, b1+2, b2, false) == 1 ); // ¯ ~ Z
			CPPUNIT_ASSERT( testType::compare(a1+3, a2, b1+3, b2, false) == 1 ); // ¥B ~ ¹B
			CPPUNIT_ASSERT( testType::compare(a1+3, a2-1, b1+3, b2-1, true) == 0 ); // ¥B ~ ¹B
			CPPUNIT_ASSERT( testType::compare(a1+3, a2-1, b1+3, b2-2, true) != 0 ); // ¥B ~ ¹B
			CPPUNIT_ASSERT( testType::compare(a1+3, a2, b1+3, b2, true) == -1 ); // ¥Bc ~ ¹Bd
		}
		{// UTF8
			std::string a = fromUnicode(L"ab¹", CP_UTF8);
			const char* a1 = a.c_str();
			const char* a2 = a1 + a.size();
			std::string b = fromUnicode(L"AB¹", CP_UTF8);
			const char* b1 = b.c_str();
			const char* b2 = b1 + b.size();

			typedef StringType<char, cpUTF8> testType;

			CPPUNIT_ASSERT( testType::compare(a1, a2, b1, b2, false) == -1 );
			CPPUNIT_ASSERT( testType::compare(a1, a2, b1, b2, true) == 0 );
		}
	}

	void testFind() {
		{
			//						 0 2 4 6 8 0 2 4
			tString a = testString(L"¹b¹B¥B¹B¹B¹B¥Bqwerty");
			const CHAR* a1 = a.c_str();
			const CHAR* a2 = a1 + a.size();
			tString b = testString(L"¹B");
			const CHAR* b1 = b.c_str();
			const CHAR* b2 = b1 + b.size();
			typedef StringType<CHAR> testType;

			// pierwszy
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, testType::find(a1, a2, b1, b2, false, 0).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::find(a1, a2, b1, b2, true, 0).getFoundPosition(a2) );
			// drugi
			CPPUNIT_ASSERT_EQUAL( (unsigned)6, testType::find(a1, a2, b1, b2, false, 1).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, testType::find(a1, a2, b1, b2, true, 1).getFoundPosition(a2) );
			// dziesiaty (za daleko)
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::find(a1, a2, b1, b2, false, 10).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::find(a1, a2, b1, b2, true, 10).getFoundPosition(a2) );
			// ostatni
			CPPUNIT_ASSERT_EQUAL( (unsigned)10, testType::find(a1, a2, b1, b2, false, -1).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)12, testType::find(a1, a2, b1, b2, true, -1).getFoundPosition(a2) );

			// nie istnieje
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::find(a1+14, a2, b1, b2, false, 0).getFoundPosition(a2) );
			// tekst mniejszy ni¿ szukany
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::find(b1, b2, a1, a2, false, 0).getFoundPosition(b2) );
		}
		{// UTF8
			// pozycja 	   		          0 2 4 6 8 0 2 4
			// dane					      0 3 6 9 2 5 8 
			std::string a = fromUnicode(L"¹b¹B¹b¹B¹B¹B¹b", CP_UTF8);
			const char* a1 = a.c_str();
			const char* a2 = a1 + a.size();
			std::string b = fromUnicode(L"¹B", CP_UTF8);
			const char* b1 = b.c_str();
			const char* b2 = b1 + b.size();

			typedef StringType<char, cpUTF8> testType;

			// pierwszy
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, testType::find(a1, a2, b1, b2, false, 0).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::find(a1, a2, b1, b2, true, 0).getFoundPosition(a2) );
			// drugi
			CPPUNIT_ASSERT_EQUAL( (unsigned)6, testType::find(a1, a2, b1, b2, false, 1).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, testType::find(a1, a2, b1, b2, true, 1).getFoundPosition(a2) );
			// dziesiaty (za daleko)
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::find(a1, a2, b1, b2, false, 10).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::find(a1, a2, b1, b2, true, 10).getFoundPosition(a2) );
			// ostatni
			CPPUNIT_ASSERT_EQUAL( (unsigned)10, testType::find(a1, a2, b1, b2, false, -1).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)12, testType::find(a1, a2, b1, b2, true, -1).getFoundPosition(a2) );
		}
	}


	void testFindChars() {
		{
			//						 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8
			tString a = testString(L"abcdef¹êæ³ABCDEF¥ÊÆ£abcdef¹êæ³");
			const CHAR* a1 = a.c_str();
			const CHAR* a2 = a1 + a.size();
			tString b = testString(L"AcE¥æ£");
			const CHAR* b1 = b.c_str();
			const CHAR* b2 = b1 + b.size();
			typedef StringType<CHAR> testType;

			// pierwszy
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, testType::findChars(a1, a2, b1, b2, false, 0).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::findChars(a1, a2, b1, b2, true, 0).getFoundPosition(a2) );
			// drugi
			CPPUNIT_ASSERT_EQUAL( (unsigned)8, testType::findChars(a1, a2, b1, b2, false, 1).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, testType::findChars(a1, a2, b1, b2, true, 1).getFoundPosition(a2) );
			// dziesiaty (za daleko)
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::findChars(a1, a2, b1, b2, false, 20).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::findChars(a1, a2, b1, b2, true, 20).getFoundPosition(a2) );
			// ostatni
			CPPUNIT_ASSERT_EQUAL( (unsigned)28, testType::findChars(a1, a2, b1, b2, false, -1).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)29, testType::findChars(a1, a2, b1, b2, true, -1).getFoundPosition(a2) );

			// nie istnieje
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::findChars(a1, a1+4, b1+3, b2, false, 0).getFoundPosition(a1+4) );
			// tekst mniejszy ni¿ szukany
			CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::findChars(b1, b2, a1, a2, false, 0).getFoundPosition(b2) );
		}
		{// UTF8
			//						      0 2 4 6 8 0 2 4 6 8 0 2 4 6 8
			std::string a = fromUnicode(L"abcdef¹êæ³ABCDEF¥ÊÆ£abcdef¹êæ³", CP_UTF8);
			const char* a1 = a.c_str();
			const char* a2 = a1 + a.size();
			std::string b = fromUnicode(L"AcE¥æ£", CP_UTF8);
			const char* b1 = b.c_str();
			const char* b2 = b1 + b.size();

			typedef StringType<char, cpUTF8> testType;

			// pierwszy
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, testType::findChars(a1, a2, b1, b2, false, 0).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::findChars(a1, a2, b1, b2, true, 0).getFoundPosition(a2) );
			// drugi
			CPPUNIT_ASSERT_EQUAL( (unsigned)8, testType::findChars(a1, a2, b1, b2, false, 1).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)2, testType::findChars(a1, a2, b1, b2, true, 1).getFoundPosition(a2) );
			// dziesiaty (za daleko)
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::findChars(a1, a2, b1, b2, false, 20).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::findChars(a1, a2, b1, b2, true, 20).getFoundPosition(a2) );
			// ostatni
			CPPUNIT_ASSERT_EQUAL( (unsigned)28, testType::findChars(a1, a2, b1, b2, false, -1).getFoundPosition(a2) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)28, testType::findChars(a1, a2, b1, b2, true, -1).getFoundPosition(a2) );
		}
	}




	template<typename STR, class CP>
	unsigned int testReplaceChars(STR& test, const STR& from, const STR& to, bool noCase, bool keepCase, bool swapMatch, int limit = -1) {
		typedef STR::value_type CH;
		StringType<CH, CP>::Iterator it = StringType<CH, CP>::replaceChars((CH*)test.c_str(), (CH*)test.c_str() + test.size(), from.c_str(), from.c_str() + from.size(), to.c_str(), to.c_str() + to.size(), noCase, keepCase, swapMatch, limit);
		*it = 0;
		stringRelease(test, it - test.c_str());
		return it.getPosition();
	}


	void testReplaceChars() {
		{ // replace
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"æ¹ba");
			tString to =   testString(L"¿óyx");
			testReplaceChars<tString, cpACP>(test, from, to, false, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"xycdABCDó¿¥Æ"), test);
		}
		{ // replace nocase
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"æ¹ba");
			tString to =   testString(L"¿óyx");
			testReplaceChars<tString, cpACP>(test, from, to, true, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"xycdxyCDó¿ó¿"), test);
		}
		{ // replace nocase keepcase
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"æ¹ba");
			tString to =   testString(L"¿óyx");
			testReplaceChars<tString, cpACP>(test, from, to, true, true, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"xycdXYCDó¿Ó¯"), test);
		}
		{ // replace swapMatch
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"abcd");
			tString to =   testString(L"1234");
			testReplaceChars<tString, cpACP>(test, from, to, false, false, true, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"abcd12341234"), test);
		}
		{ // replace swapMatch nocase
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"abcd");
			tString to =   testString(L"1234");
			testReplaceChars<tString, cpACP>(test, from, to, true, false, true, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"abcdABCD1234"), test);
		}
		{ // replace limit
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"cdab");
			tString to =   testString(L"xyzw");
			testReplaceChars<tString, cpACP>(test, from, to, false, false, false, 2);
			CPPUNIT_ASSERT_EQUAL(testString(L"zwcdABCD¹æ¥Æ"), test);
		}
		{ // replace swapMatch limit
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"abcd");
			tString to =   testString(L"1234");
			testReplaceChars<tString, cpACP>(test, from, to, false, false, true, 2);
			CPPUNIT_ASSERT_EQUAL(testString(L"abcd12CD¹æ¥Æ"), test);
		}
		{ // replace erase
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"aA¹¥");
			tString to =   testString(L"");
			testReplaceChars<tString, cpACP>(test, from, to, false, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"bcdBCDæÆ"), test);
		}
		{ // replace swapMatch erase
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"aA¹¥");
			tString to =   testString(L"");
			testReplaceChars<tString, cpACP>(test, from, to, false, false, true, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"aA¹¥"), test);
		}

		{ // replace last
			tString test = testString(L"abcdABCD¹æ¥Æ");
			tString from = testString(L"aA¹¥Æ");
			tString to =   testString(L"eE ");
			testReplaceChars<tString, cpACP>(test, from, to, false, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"ebcdEBCD æ  "), test);
		}

		{ // replace UTF8 erase
			std::string test = fromUnicode(L"B¥Kêd", CP_UTF8);
			std::string from = fromUnicode(L"¹êæ¥ÊÆ", CP_UTF8);
			std::string to =   fromUnicode(L"", CP_UTF8);
			testReplaceChars<std::string, cpUTF8>(test, from, to, false, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(fromUnicode(L"BKd", CP_UTF8), test);
		}

		{ // replace UTF8
			std::string test = fromUnicode(L"B¥Kêd", CP_UTF8);
			std::string from = fromUnicode(L"¹êæ¥ÊÆ", CP_UTF8);
			std::string to =   fromUnicode(L"¥ÊÆ¹êæ", CP_UTF8);
			testReplaceChars<std::string, cpUTF8>(test, from, to, false, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(fromUnicode(L"B¹KÊd", CP_UTF8), test);
		}

		{ // replace UTF8 smaller
			std::string test = fromUnicode(L"B¥Kêd", CP_UTF8);
			std::string from = fromUnicode(L"¹êæ¥ÊÆ", CP_UTF8);
			std::string to =   fromUnicode(L"aecAEC", CP_UTF8);
			testReplaceChars<std::string, cpUTF8>(test, from, to, false, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(fromUnicode(L"BAKed", CP_UTF8), test);
		}

	}

	void testMakeUpper() {
		typedef StringType<CHAR> testType;
		{
			tString test = testString(L"abCD¹êÆ£");
			testType::makeUpper((CHAR*)test.c_str(), (CHAR*)test.c_str() + test.size());
			CPPUNIT_ASSERT_EQUAL(testString(L"ABCD¥ÊÆ£"), test);
		}
		{ // UTF8
			std::string test = fromUnicode(L"abCD¹êÆ£", CP_UTF8);
			StringType<char, cpUTF8>::makeUpper((char*)test.c_str(), (char*)test.c_str() + test.size());
			CPPUNIT_ASSERT_EQUAL(fromUnicode(L"ABCD¹êÆ£", CP_UTF8), test);
		}
	}

	void testMakeLower() {
		typedef StringType<CHAR> testType;
		{
			tString test = testString(L"abCD¹êÆ£");
			testType::makeLower((CHAR*)test.c_str(), (CHAR*)test.c_str() + test.size());
			CPPUNIT_ASSERT_EQUAL(testString(L"abcd¹êæ³"), test);
		}
		{ // UTF8
			std::string test = fromUnicode(L"abCD¹êÆ£", CP_UTF8);
			StringType<char, cpUTF8>::makeLower((char*)test.c_str(), (char*)test.c_str() + test.size());
			CPPUNIT_ASSERT_EQUAL(fromUnicode(L"abcd¹êÆ£", CP_UTF8), test);
		}
	}


};


CPPUNIT_TEST_SUITE_REGISTRATION( TestStringType<char> );
CPPUNIT_TEST_SUITE_REGISTRATION( TestStringType<wchar_t> );
