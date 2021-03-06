/*

The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License from
/LICENSE.HTML in this package or at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is "Stamina.lib" library code, released Feb 1, 2006.

The Initial Developer of the Original Code is "STAMINA" - Rafa� Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa� Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id: $

*/

#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina/VersionControl.h>
#include <Stamina/WideChar.h>
#include <Stamina/StringType.h>
//#include <Stamina/String.h>
#include <ConvertUTF.h>

using namespace Stamina;

template <class TYPE>
class TestStringType : public CPPUNIT_NS::TestFixture
{

	typedef typename TYPE::tChar CHAR;
  
	const static bool isUTF8 = TYPE::codepage == CP_UTF8;

	CPPUNIT_TEST_SUITE( TestStringType<TYPE> );
  
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
		static std::wstring s(L"abc���123");
		return testString(s);
	}

	tString upperString() {
		static std::wstring s(L"ABC���123");
		return testString(s);
	}

	tString testString(std::wstring s) {
		if (TYPE::codepage != CP_ACP) {
			return keepChar<std::basic_string<CHAR> >( fromUnicode(s, TYPE::codepage) );
		} else {
			return keepChar<std::basic_string<CHAR> >(s);
		}
	}

protected:


	void testIterator() {
		tString test = lowerString();
		TYPE::ConstIterator it(test.c_str());
		CPPUNIT_ASSERT( it == test.c_str() );
		CPPUNIT_ASSERT( it.getPosition() == 0 );

		while (it < (test.c_str() + test.size())) {
			++it;
		}

		CPPUNIT_ASSERT( it.getDataPosition(test.c_str()) == test.size() );
		CPPUNIT_ASSERT( it.getPosition() <= test.size() );
		CPPUNIT_ASSERT( it.getFoundPosition(test.c_str() + test.size()) == stACP::notFound );

		{
			tString toStr = testString(L"�BC");
			tString fromStr = testString(L"�bc");
			TYPE::Iterator to((CHAR*)toStr.c_str());
			TYPE::ConstIterator from(fromStr.c_str());
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
		tString upper = testString(L"�ʌ�AB12");
		tString lower = testString(L"���ab12");
		TYPE::ConstIterator u(upper.c_str());
		TYPE::ConstIterator l(lower.c_str());

		for (; u < upper.c_str() + upper.size(); ++u, ++l) {
			if (u.canChangeCase() && l.canChangeCase()) {
				CPPUNIT_ASSERT( u.getLower() == l.character() );
				CPPUNIT_ASSERT( l.getUpper() == u.character() );
				CPPUNIT_ASSERT( u.getLower() == l.getLower() );
				CPPUNIT_ASSERT( u.getUpper() == l.getUpper() );
			} else {
				CPPUNIT_ASSERT( u.getLower() == u.character() );
				CPPUNIT_ASSERT( l.getUpper() == l.character() );
			}
		}
	}

	void testIteratorCompare() {
		tString test1 = testString(L"�");
		tString test2 = testString(L"�");
		tString test3 = testString(L"B");
		tString test4 = testString(L"b");

		TYPE::ConstIterator a;
		TYPE::ConstIterator b;

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
		if (TYPE::codepage != CP_UTF8) return;
		tString test = testString(L"��d�");
		TYPE::ConstIterator it(test.c_str());
		CPPUNIT_ASSERT( it == test.c_str() );
		CPPUNIT_ASSERT( it.getPosition() == 0 );

		for (; it < test.c_str() + test.size(); ++it) {
		}

		CPPUNIT_ASSERT( it.getPosition() == 4 );
		CPPUNIT_ASSERT( it - test.c_str() == 7 );

		it -= 4; // �
		it += 2; // d
		CPPUNIT_ASSERT( it.getPosition() == 2 );
		CPPUNIT_ASSERT_EQUAL( (char)'d', (char)*it );

		tString lower = testString(L"�");
		it = test.c_str();
		__int64 character = it.character();
		__int64 buff = it.getLower();
		std::wstring result = toUnicode((char*)&buff, CP_UTF8);
		//CPPUNIT_ASSERT_EQUAL( std::wstring(L"�"), result );
		//CPPUNIT_ASSERT_EQUAL( lower, std::string((char*)&buff) );
		// Zmiana rozmiaru jest dozwolona dla UTF-8
		CPPUNIT_ASSERT_EQUAL( std::wstring(L"�"), result );
		it += 2; // d
		buff = it.getUpper();
		result = toUnicode((char*)&buff, CP_UTF8);
		CPPUNIT_ASSERT_EQUAL( std::wstring(L"D"), result );

		{
			tString toStr = testString(L"�BC");
			tString fromStr = testString(L"�bc");
			TYPE::Iterator to((CHAR*)toStr.c_str());
			TYPE::ConstIterator from(fromStr.c_str());
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

	void testGetLength() {
		tString test = testString(L"abc���");
		CPPUNIT_ASSERT_EQUAL( (unsigned)6, TYPE::getLength(test.c_str(), test.c_str() + test.size()) );

	}

	void testGetDataPos() {
		tString test = testString(L"abc���");
		const CHAR* from = test.c_str();
		const CHAR* to = from + test.size();

		typedef TYPE testType;

		if (TYPE::codepage != CP_UTF8) {

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
		} else {

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
		tString test = testString(L"abc���");
		const CHAR* from = test.c_str();
		const CHAR* to = from + test.size();

		typedef TYPE testType;

		if (TYPE::codepage != CP_UTF8) {

			CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getCharPos(from, to, 0) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)4, testType::getCharPos(from, to, 4) );
			// przesadzony
			CPPUNIT_ASSERT_EQUAL( (unsigned)6, testType::getCharPos(from, to, 20) );
		} else { // UTF8

			CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::getCharPos(from, to, 0) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)4, testType::getCharPos(from, to, 5) );
			CPPUNIT_ASSERT_EQUAL( (unsigned)4, testType::getCharPos(from, to, 6) );
			// przesadzony
			CPPUNIT_ASSERT_EQUAL( (unsigned)6, testType::getCharPos(from, to, 20) );

		}
	}

	void testEqual() {
		{
			tString a = testString(L"abc���");
			const CHAR* a1 = a.c_str();
			const CHAR* a2 = a1 + a.size();
			tString b = testString(L"ABC���");
			const CHAR* b1 = b.c_str();
			const CHAR* b2 = b1 + b.size();

			typedef TYPE testType;

			CPPUNIT_ASSERT( testType::equal(a1, a2, b1, b2, false) == false );
			CPPUNIT_ASSERT( testType::equal(a1, a2, b1, b2, true) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a2, a1, a2, false) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a2, a1, a2, true) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a1, a1, a1, false) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a1, b1, b1, false) == true );
			CPPUNIT_ASSERT( testType::equal(a1, a2, a1, a2 - 2, true) == false );
		}

	}

	void testCompare() {
		{
			tString a = testString(L"a���Bc");
			TYPE::ConstIterator a1 = a.c_str();
			TYPE::ConstIterator a2 = a.c_str() + a.size();
			tString b = testString(L"abZ�Bd");
			TYPE::ConstIterator b1 = b.c_str();
			TYPE::ConstIterator b2 = b.c_str() + b.size();
			tString c = testString(L"abZ");
			TYPE::ConstIterator c1 = c.c_str();
			TYPE::ConstIterator c2 = c.c_str() + c.size();



			typedef TYPE testType;

			CPPUNIT_ASSERT( testType::compare(a1(), a2(), b1(), b2(), false) == -1 );
			CPPUNIT_ASSERT( testType::compare((a1+2)(), a2(), (b1+2)(), b2(), false) == 1 ); // � ~ Z
			CPPUNIT_ASSERT( testType::compare((a1+3)(), a2(), (b1+3)(), b2(), false) != 0 ); // �B ~ �B
			CPPUNIT_ASSERT( testType::compare((a1+3)(), (a2-1)(), (b1+3)(), (b2-1)(), true) == 0 ); // �B ~ �B
			CPPUNIT_ASSERT( testType::compare((a1+3)(), (a2-1)(), (b1+3)(), (b2-2)(), true) != 0 ); // �B ~ �B
			CPPUNIT_ASSERT( testType::compare((a1+3)(), a2(), (b1+3)(), b2(), true) == -1 ); // �Bc ~ �Bd

			CPPUNIT_ASSERT( testType::compare(b1(), b2(), c1(), c2(), false) == 1 ); // 
			CPPUNIT_ASSERT( testType::compare(c1(), c2(), b1(), b2(), false) == -1 ); // 
			CPPUNIT_ASSERT( testType::compare(a1(), a2(), c1(), c2(), false) == -1 ); // 

		}
	}

	void testFind() {
		{
			//						 0 2 4 6 8 0 2 4
			tString a = testString(L"�b�B�B�B�B�B�Bqwerty");
			const CHAR* a1 = a.c_str();
			const CHAR* a2 = a1 + a.size();
			tString b = testString(L"�B");
			const CHAR* b1 = b.c_str();
			const CHAR* b2 = b1 + b.size();
			typedef TYPE testType;

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
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::find((TYPE::ConstIterator(a1)+14)(), a2, b1, b2, false, 0).getFoundPosition(a2) );
			// tekst mniejszy ni� szukany
			CPPUNIT_ASSERT_EQUAL( (unsigned)-1, testType::find(b1, b2, a1, a2, false, 0).getFoundPosition(b2) );
		}
	}


	void testFindChars() {
		{
			//						 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8
			tString a = testString(L"abcdef���ABCDEF��ƣabcdef���");
			const CHAR* a1 = a.c_str();
			const CHAR* a2 = a1 + a.size();
			tString b = testString(L"AcE��");
			const CHAR* b1 = b.c_str();
			const CHAR* b2 = b1 + b.size();
			typedef TYPE testType;

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
			// tekst mniejszy ni� szukany
			CPPUNIT_ASSERT_EQUAL( (unsigned)0, testType::findChars(b1, b2, a1, a2, false, 0).getFoundPosition(b2) );
		}
	}




	unsigned int testReplaceChars(tString& test, const tString& from, const tString& to, bool noCase, bool keepCase, bool swapMatch, int limit = -1) {
		TYPE::Iterator it = TYPE::replaceChars((CHAR*)test.c_str(), (CHAR*)test.c_str() + test.size(), from.c_str(), from.c_str() + from.size(), to.c_str(), to.c_str() + to.size(), noCase, keepCase, swapMatch, limit);
		*it = 0;
		stringRelease(test, it - test.c_str());
		return it.getPosition();
	}


	void testReplaceChars() {
		{ // replace
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"�ba");
			tString to =   testString(L"��yx");
			testReplaceChars(test, from, to, false, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"xycdABCD��"), test);
		}
		{ // replace nocase
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"�ba");
			tString to =   testString(L"��yx");
			testReplaceChars(test, from, to, true, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"xycdxyCD��"), test);
		}
		{ // replace nocase keepcase
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"�ba");
			tString to =   testString(L"��yx");
			testReplaceChars(test, from, to, true, true, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"xycdXYCD�ӯ"), test);
		}
		{ // replace swapMatch
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"abcd");
			tString to =   testString(L"1234");
			testReplaceChars(test, from, to, false, false, true, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"abcd12341234"), test);
		}
		{ // replace swapMatch nocase
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"abcd");
			tString to =   testString(L"1234");
			testReplaceChars(test, from, to, true, false, true, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"abcdABCD1234"), test);
		}
		{ // replace limit
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"cdab");
			tString to =   testString(L"xyzw");
			testReplaceChars(test, from, to, false, false, false, 2);
			CPPUNIT_ASSERT_EQUAL(testString(L"zwcdABCD���"), test);
		}
		{ // replace swapMatch limit
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"abcd");
			tString to =   testString(L"1234");
			testReplaceChars(test, from, to, false, false, true, 2);
			CPPUNIT_ASSERT_EQUAL(testString(L"abcd12CD���"), test);
		}
		{ // replace erase
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"aA��");
			tString to =   testString(L"");
			testReplaceChars(test, from, to, false, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"bcdBCD��"), test);
		}
		{ // replace swapMatch erase
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"aA��");
			tString to =   testString(L"");
			testReplaceChars(test, from, to, false, false, true, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"aA��"), test);
		}

		{ // replace last
			tString test = testString(L"abcdABCD���");
			tString from = testString(L"aA���");
			tString to =   testString(L"eE ");
			testReplaceChars(test, from, to, false, false, false, -1);
			CPPUNIT_ASSERT_EQUAL(testString(L"ebcdEBCD �  "), test);
		}


	}

	void testMakeUpper() {
		typedef TYPE testType;
		{
			tString test = testString(L"abCD��ƣ");
			testType::makeUpper((CHAR*)test.c_str(), (CHAR*)test.c_str() + test.size());
			CPPUNIT_ASSERT_EQUAL(testString(L"ABCD��ƣ"), test);
		}
	}

	void testMakeLower() {
		typedef TYPE testType;
		{
			tString test = testString(L"abCD��ƣ");
			testType::makeLower((CHAR*)test.c_str(), (CHAR*)test.c_str() + test.size());
			CPPUNIT_ASSERT_EQUAL(testString(L"abcd���"), test);
		}
	}


};


CPPUNIT_TEST_SUITE_REGISTRATION( TestStringType< stACP > );
CPPUNIT_TEST_SUITE_REGISTRATION( TestStringType< stUNICODE > );
CPPUNIT_TEST_SUITE_REGISTRATION( TestStringType< stUTF8 > );
