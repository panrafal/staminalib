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

The Initial Developer of the Original Code is "STAMINA" - Rafa³ Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa³ Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

*/

#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <Stamina/VersionControl.h>
#include <Stamina/WideChar.h>
#include <Stamina/Buffer.h>
//#include <Stamina/String.h>
#include <ConvertUTF.h>

using namespace Stamina;
using std::cout;
using std::endl;

template <typename CHAR>
class TestStringBuffer : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestStringBuffer<CHAR> );
  
	CPPUNIT_TEST( testConstruction );
	CPPUNIT_TEST( testCheapReference );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testResize );
	CPPUNIT_TEST( testMakeRoom );
	CPPUNIT_TEST( testMoveLeft );
	CPPUNIT_TEST( testMoveRight );
	CPPUNIT_TEST( testAppend );
	CPPUNIT_TEST( testPrepend );
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testErase );
	CPPUNIT_TEST( testTruncate );
	CPPUNIT_TEST( testMakeUnique );
	CPPUNIT_TEST( testDiscard );
	CPPUNIT_TEST( testReset );
	CPPUNIT_TEST( testSwap );
	CPPUNIT_TEST( testReplace );
	CPPUNIT_TEST( testPassBuffer );
	CPPUNIT_TEST( testBufferRef );

	CPPUNIT_TEST_SUITE_END();

protected:

	typedef std::basic_string<CHAR> tString;
	typedef Buffer<CHAR> tBuffer;

public:

	void setUp() {

	}
	void tearDown() {
	}

	tString shortString() {
		static std::string s;
		if (s.empty()) {
			s.resize(30);
			for (unsigned int i = 0; i < 30; ++i) {
				s[i] = '0' + (i % 24);
			}
		}
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tString mediumString() {
		static std::string s;
		if (s.empty()) {
			s.resize(70);
			for (unsigned int i = 0; i < 70; ++i) {
				s[i] = 'a' + (i % 24);
			}
		}
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tString longString() {
		static std::string s;
		if (s.empty()) {
			s.resize(500);
			for (unsigned int i = 0; i < 500; ++i) {
				s[i] = 'A' + (i % 24);
			}
		}
		return keepChar<std::basic_string<CHAR> >(s);
	}

	static const unsigned int poolSmall = 32;
	static const unsigned int poolMedium = 128;


protected:

	void testConstruction() {

		{
		Buffer<CHAR> b1;
		CPPUNIT_ASSERT( b1.isEmpty() == true );
		CPPUNIT_ASSERT( b1.isValid() == false );
		CPPUNIT_ASSERT( b1.isReference() == false );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == false );
		CPPUNIT_ASSERT( b1.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b1.getLength() == 0 );
		}
		{
		Buffer<CHAR> b2(10);
		CPPUNIT_ASSERT( b2.isEmpty() == false );
		CPPUNIT_ASSERT( b2.isValid() == false );
		CPPUNIT_ASSERT( b2.isReference() == false );
		CPPUNIT_ASSERT( b2.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b2.getBufferSize() == 32 );
		CPPUNIT_ASSERT( b2.getLength() == 0 );
		}
		{
		Buffer<CHAR> b3(60);
		CPPUNIT_ASSERT( b3.isEmpty() == false );
		CPPUNIT_ASSERT( b3.isValid() == false );
		CPPUNIT_ASSERT( b3.isReference() == false );
		CPPUNIT_ASSERT( b3.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b3.getBufferSize() == 128 );
		CPPUNIT_ASSERT( b3.getLength() == 0 );
		}
		{
		Buffer<CHAR> b4(1025);
		CPPUNIT_ASSERT( b4.isEmpty() == false );
		CPPUNIT_ASSERT( b4.isValid() == false );
		CPPUNIT_ASSERT( b4.isReference() == false );
		CPPUNIT_ASSERT( b4.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b4.getBufferSize() == 1025 );
		CPPUNIT_ASSERT( b4.getLength() == 0 );
		}
	}

	void testCheapReference() {
		tString sRef = shortString();
		const CHAR* ref = sRef.c_str();

		// clean
		{
		Buffer<CHAR> b1;
		b1.assignCheapReference(ref);
		CPPUNIT_ASSERT( b1.isEmpty() == false );
		CPPUNIT_ASSERT( b1.isValid() == true );
		CPPUNIT_ASSERT( b1.isReference() == true );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == false );
		CPPUNIT_ASSERT( b1.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b1.getLength() == sRef.length() );
		CPPUNIT_ASSERT_EQUAL( (CHAR*)ref, (CHAR*)b1.getBuffer() );
		}
		// after buffer
		{
		Buffer<CHAR> b2(128);
		b2.assignCheapReference(ref);
		CPPUNIT_ASSERT( b2.isEmpty() == false );
		CPPUNIT_ASSERT( b2.isValid() == true );
		CPPUNIT_ASSERT( b2.isReference() == true );
		CPPUNIT_ASSERT( b2.hasOwnBuffer() == false );
		CPPUNIT_ASSERT( b2.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b2.getLength() == sRef.length() );
		CPPUNIT_ASSERT_EQUAL( (CHAR*)ref, (CHAR*)b2.getBuffer() );
		}

		CPPUNIT_ASSERT_EQUAL( shortString(), tString(ref) );
	}

	void testAssign() {
		// clean - short
		{
		Buffer<CHAR> b1;
		b1.assign(shortString().c_str(), shortString().size());
		CPPUNIT_ASSERT( b1.isEmpty() == false );
		CPPUNIT_ASSERT( b1.isValid() == true );
		CPPUNIT_ASSERT( b1.isReference() == false );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b1.getBufferSize() == poolSmall );
		CPPUNIT_ASSERT( b1.getLength() == shortString().length() );
		CPPUNIT_ASSERT_EQUAL( shortString(), tString(b1.getBuffer()) );
		}
		// reuse - short
		{
		Buffer<CHAR> b2(poolSmall);
		b2.assign(shortString().c_str(), shortString().size());
		CPPUNIT_ASSERT( b2.isValid() == true );
		CPPUNIT_ASSERT( b2.getBufferSize() == poolSmall );
		CPPUNIT_ASSERT( b2.getLength() == shortString().length() );
		CPPUNIT_ASSERT_EQUAL( shortString(), tString(b2.getBuffer()) );
		}

		// clean - medium
		{
		Buffer<CHAR> b3;
		b3.assign(mediumString().c_str(), mediumString().size());
		CPPUNIT_ASSERT_EQUAL( poolMedium, b3.getBufferSize() );
		CPPUNIT_ASSERT_EQUAL( mediumString().length(), b3.getLength() );
		CPPUNIT_ASSERT_EQUAL( mediumString(), tString(b3.getBuffer()) );
		}

		// clean - large
		{
		Buffer<CHAR> b4;
		b4.assign(longString().c_str(), longString().size());
		CPPUNIT_ASSERT( b4.getBufferSize() >= longString().length() );
		CPPUNIT_ASSERT_EQUAL( longString().length(), b4.getLength() );
		CPPUNIT_ASSERT_EQUAL( longString(), tString(b4.getBuffer()) );
		}

		// reuse - large
		{
		Buffer<CHAR> b5(5000);
		b5.assign(longString().c_str(), longString().size());
		CPPUNIT_ASSERT_EQUAL( (unsigned int)5000, b5.getBufferSize() );
		CPPUNIT_ASSERT_EQUAL( longString().length(), b5.getLength() );
		CPPUNIT_ASSERT_EQUAL( longString(), tString(b5.getBuffer()) );
		}
	}

	void testResize() {
		// na czystym buforze - sprawdzamy czy go nie zvaliduje...
		{
		Buffer<CHAR> b1;
		b1.resize(20);
		CPPUNIT_ASSERT( b1.isValid() == false );
		CPPUNIT_ASSERT_EQUAL( poolSmall, b1.getBufferSize() );
		b1.resize(1000);
		CPPUNIT_ASSERT( b1.isValid() == false );
		CPPUNIT_ASSERT_EQUAL( (unsigned int)1000, b1.getBufferSize() );
		b1.resize(0);
		CPPUNIT_ASSERT( b1.isEmpty() == true );
		CPPUNIT_ASSERT( b1.isValid() == false );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == false );
		CPPUNIT_ASSERT_EQUAL( (unsigned int)0, b1.getBufferSize() );
		}
		// na pe³nym buforze - sprawdzamy kopiowanie
		{
		Buffer<CHAR> b;
		b.assign(shortString().c_str(), shortString().size());
		CPPUNIT_ASSERT( b.isValid() == true );
		b.resize(100);
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT_EQUAL( shortString().size(), b.getLength() );
		CPPUNIT_ASSERT_EQUAL( shortString(), tString(b.getBuffer()) );
		b.resize(1000);
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT_EQUAL( shortString().size(), b.getLength() );
		CPPUNIT_ASSERT_EQUAL( shortString(), tString(b.getBuffer()) );
		b.resize(5);
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT_EQUAL( (unsigned)5, b.getLength() );
		CPPUNIT_ASSERT_EQUAL( shortString().substr(0,5), tString(b.getBuffer()) );
		}

		// na pe³nym buforze - sprawdzamy szcz¹tkowe kopiowanie
		{
		Buffer<CHAR> b;
		b.assign(shortString().c_str(), shortString().size());
		b.resize(100, 10);
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT_EQUAL( (unsigned)10, b.getLength() );
		CPPUNIT_ASSERT_EQUAL( shortString().substr(0,10), tString(b.getBuffer()) );
		b.resize(1000, 20); // za du¿e - rozmiary powinny zostaæ te same
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT_EQUAL( (unsigned)10, b.getLength() );
		CPPUNIT_ASSERT_EQUAL( shortString().substr(0,10), tString(b.getBuffer()) );
		}
		// na pe³nym buforze - sprawdzamy szcz¹tkowe kopiowanie niezgodne z rozmiarem bufora
		{
		Buffer<CHAR> b;
		b.assign(longString().c_str(), longString().size());
		b.resize(longString().size() - 100, longString().size()); // kopia wiêksza ni¿ bufor
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT_EQUAL( longString().size() - 100, b.getLength() );
		CPPUNIT_ASSERT_EQUAL( longString().substr(0, b.getLength()), tString(b.getBuffer()) );
		b.resize(0, 200);
		CPPUNIT_ASSERT( b.isEmpty() == true );
		CPPUNIT_ASSERT( b.isValid() == false );
		CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
		CPPUNIT_ASSERT_EQUAL( (unsigned int)0, b.getBufferSize() );
		}
	}

	void testMakeRoom() {
		{ // na czysto
			Buffer<CHAR> b;
			CHAR* buff = b.getBuffer();
			b.makeRoom(poolSmall - 2);
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.getBufferSize() == poolSmall );
			CPPUNIT_ASSERT( b.getBuffer() != buff );
			buff = b.getBuffer();
			b.assign(shortString().c_str(), shortString().size());
			b.makeRoom(poolMedium - 2);
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isValid() == true );
			CPPUNIT_ASSERT( b.getBufferSize() == poolMedium );
			CPPUNIT_ASSERT( b.getBuffer() != buff );
			buff = b.getBuffer();
			b.makeRoom(poolSmall - 2); // zmniejszamy - bez zmian
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isValid() == true );
			CPPUNIT_ASSERT( b.getBufferSize() == poolMedium );
			CPPUNIT_ASSERT( b.getBuffer() == buff );
			buff = b.getBuffer();
			b.makeRoom(5000); // zwiekszamy znacznie - bufor sie dopasowuje
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isValid() == true );
			CPPUNIT_ASSERT( b.getBufferSize() == 5000 );
			CPPUNIT_ASSERT( b.getBuffer() != buff );
			buff = b.getBuffer();
			b.makeRoom(5010); // zwiekszamy troche - bufor powinien podskoczyæ mocniej
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isValid() == true );
			CPPUNIT_ASSERT( b.getBufferSize() > 5010 );
			CPPUNIT_ASSERT( b.getBuffer() != buff );
			buff = b.getBuffer();
			b.makeRoom(5100); // zwiekszamy troche - przy tym rozmiarze bufora powinno wystarczyæ
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isValid() == true );
			CPPUNIT_ASSERT( b.getBufferSize() > 5100 );
			CPPUNIT_ASSERT( b.getBuffer() == buff );
			buff = b.getBuffer();
			b.makeRoom(0); // zerujemy - room wystarczy - wiêc nie ma wyniku
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isValid() == true );
			CPPUNIT_ASSERT( b.getBufferSize() > 5100 );
			CPPUNIT_ASSERT( b.getBuffer() == buff );
			buff = b.getBuffer();
			b.makeRoom(3000); // dalej wystarcza
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isValid() == true );
			CPPUNIT_ASSERT( b.getBufferSize() > 3000 );
			CPPUNIT_ASSERT( b.getBuffer() == buff );
			buff = b.getBuffer();
		}
		{ // z reference
			Buffer<CHAR> b;
			tString ref = shortString();
			b.assignCheapReference(ref.c_str());
			b.makeRoom(poolSmall);
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isValid() == true );
			CPPUNIT_ASSERT( b.getBufferSize() == poolSmall );
			CPPUNIT_ASSERT_EQUAL( shortString(), tString(b.getBuffer()) );
		}
	}

	void testMoveLeft(bool byRef) {
		tString test = shortString();
		size_t size = test.size();
		const CHAR* testRef = test.c_str();
		{ // przesuwamy ca³y tekst od pocz¹tku
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 5);
			CPPUNIT_ASSERT_EQUAL( test.substr(5), tString(b.getString()) );
		}
		{ // przesuwamy kawa³ek tekstu od pocz¹tku - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			// 0123456789012345678901234567890
			// 2343456789012345678901234567890
			b.moveLeft(0, 2, 5, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(2, 3) + test.substr(3), tString(b.getString()) );
		}
		{ // przesuwamy ca³y tekst od pocz¹tku - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 5, Buffer<CHAR>::wholeData, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(5) + test.substr(size - 5), tString(b.getString()) );
		}
		{ // przesuwamy kawa³ek tekstu od pocz¹tku - z truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			// 0123456789012345678901234567890
			// 2343456789012345678901234567890
			b.moveLeft(0, 2, 5, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(2, 3), tString(b.getString()) );
		}
		{ // przesuwamy ca³y tekst zostawiaj¹c dwie litery na pocz¹tku
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(7, 5);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2) + test.substr(7), tString(b.getString()) );
		}
		{ // przesuwamy kilka liter w œrodku - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(7, 5, 5, false);
			// 123456789012345678901234567890
			// 128901289012345678901234567890
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2) + test.substr(7, 5) + test.substr(7), tString(b.getString()) );
		}
		{ // przesuwamy jedn¹ literê w œrodku - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(1, 1, 1, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(1, 1) + test.substr(1, 1) + test.substr(2), tString(b.getString()) );
		}
		{ // przesuwamy poza zakres
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, size + 2);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // przesuwamy poza zakres zbyt krotkim ciagiem bez truncate - nic siê nie powinno zmienic
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 5, false);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // przesuwamy poza zakres zbyt krotkim ciagiem z truncate - powinien sie wyczyscic
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 5, true);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // przesuwamy, ale zero znaków - z truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 0, true);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // przesuwamy ze œrodka, ale zero znaków - z truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(12, 10, 0, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2), tString(b.getString()) );
		}
		{ // przesuwamy, ale zero znaków - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 0, false);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}		
		{ // nie ruszamy
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(5, 0);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // przesuwamy za d³ugi tekst
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(5, 10, size*2);
			CPPUNIT_ASSERT_EQUAL( test.substr(10), tString(b.getString()) );
		}
		{ // przesuwamy pust¹ referencjê
			Buffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.moveLeft(0, 2);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
	}

	void testMoveLeft() {
		cout << " NoRef ";
		testMoveLeft(false);
		cout << " Ref ";
		testMoveLeft(true);
	}

	void testMoveRight(bool byRef) {
		tString test = shortString();
		size_t size = test.size();
		const CHAR* testRef = test.c_str();
		{ // przesuwamy ca³y tekst od pocz¹tku
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, 5);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test, tString(b.getString()) );
		}
		{ // przesuwamy kawa³ek tekstu od pocz¹tku - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			// 0123456789012345678901234567890
			// 0101234789012345678901234567890
			b.moveRight(0, 2, 5, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2) + test.substr(0, 5) + test.substr(7), tString(b.getString()) );
		}
		{ // przesuwamy ca³y tekst od pocz¹tku - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, 5, Buffer<CHAR>::wholeData, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test, tString(b.getString()) );
		}
		{ // przesuwamy kawa³ek tekstu od pocz¹tku - z truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			// 0123456789012345678901234567890
			// 0101234
			b.moveRight(0, 2, 5, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2) + test.substr(0, 5), tString(b.getString()) );
		}
		{ // przesuwamy ca³y tekst zostawiaj¹c dwie litery na koñcu
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, 5, size - 7, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test.substr(0, size - 7) + test.substr(size - 2, 2), tString(b.getString()) );
		}
		{ // przesuwamy kilka liter w œrodku - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(7, 5, 5, false);
			// 0123456789012345678901234567890
			// 0123456789017890178901234567890
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 12) + test.substr(7, 5) + test.substr(17), tString(b.getString()) );
		}
		{ // przesuwamy jedn¹ literê w œrodku - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(1, 1, 1, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 1) + test.substr(1, 1) + test.substr(1, 1) + test.substr(3), tString(b.getString()) );
		}
		{ // przesuwamy poza zakres
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, size);
			CPPUNIT_ASSERT_EQUAL( test + test, tString(b.getString()) );
		}
		{ // przesuwamy poza zakres krotkim ciagiem
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, size, 5);
			CPPUNIT_ASSERT_EQUAL( test + test.substr(0, 5), tString(b.getString()) );
		}
		{ // przesuwamy, ale zero znaków - z truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, 10, 0, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 10), tString(b.getString()) );
		}
		{ // przesuwamy ze œrodka, ale zero znaków - z truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(2, 10, 0, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 12), tString(b.getString()) );
		}
		{ // przesuwamy, ale zero znaków - bez truncate
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 0, false);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}		
		{ // nie ruszamy
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(5, 0);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // przesuwamy za d³ugi tekst
			Buffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(5, 10, size*2);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 15) + test.substr(5), tString(b.getString()) );
		}
		{ // przesuwamy pust¹ referencjê
			Buffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.moveRight(0, 1);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}

	}

	void testMoveRight() {
		cout << " NoRef ";
		testMoveRight(false);
		cout << " Ref ";
		testMoveRight(true);
	}

	void testAppend() {
		tString test1 = shortString();
		tString test2 = mediumString();
		{
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.append(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1 + test2, tString(b.getString()) );
		}
		{ // do pustego
 			Buffer<CHAR> b;
			b.append(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // do pustej referencji
			Buffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.append(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // dodajemy puste
			Buffer<CHAR> b; 
			b.assign(test1.c_str(), test1.size());
			b.append((CHAR*)L"", 0);
			CPPUNIT_ASSERT_EQUAL( test1, tString(b.getString()) );
		}
	}

	void testPrepend() {
		tString test1 = shortString();
		tString test2 = mediumString();
		{
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.prepend(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2 + test1, tString(b.getString()) );
		}
		{ // do pustego
			Buffer<CHAR> b;
			b.prepend(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // do pustej referencji
			Buffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.prepend(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // dodajemy puste
			Buffer<CHAR> b; 
			b.assign(test1.c_str(), test1.size());
			b.prepend((CHAR*)L"", 0);
			CPPUNIT_ASSERT_EQUAL( test1, tString(b.getString()) );
		}
	}

	void testInsert() {
		tString test1 = shortString();
		tString test2 = mediumString();
		{ // na pocz¹tek
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.insert(0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2 + test1, tString(b.getString()) );
		}
		{ // w œrodek
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.insert(10, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2 + test1.substr(10), tString(b.getString()) );
		}
		{ // na koniec
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.insert(b.getLength(), test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1 + test2, tString(b.getString()) );
		}
		{ // do pustego na pocz¹tek
			Buffer<CHAR> b;
			b.insert(0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // dodajemy puste
			Buffer<CHAR> b; 
			b.assign(test1.c_str(), test1.size());
			b.insert(10, (CHAR*)L"", 0);
			CPPUNIT_ASSERT_EQUAL( test1, tString(b.getString()) );
		}
		{ // wstawiamy poza
			Buffer<CHAR> b; 
			b.assign(test1.c_str(), test1.size());
			b.insert(b.getLength() * 2, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.size()*2 + test2.size(), b.getLength() );
		}
		{ // do pustej referencji
			Buffer<CHAR> b; 
			b.assignCheapReference((CHAR*)L"", 0);
			b.insert(0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
	}

	void testReplace() {
		tString test1 = mediumString();
		tString test2 = shortString();
		{ // na pocz¹tku - zerowy
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(0, 0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2 + test1, tString(b.getString()) );
		}
		{ // na pocz¹tku
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(0, 5, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2 + test1.substr(5), tString(b.getString()) );
		}
		{ // w œrodek - krótszy
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 10, test2.c_str(), 5);
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2.substr(0,5) + test1.substr(20), tString(b.getString()) );
		}
		{ // w œrodek - równy
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 10, test2.c_str(), 10);
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2.substr(0,10) + test1.substr(20), tString(b.getString()) );
		}
		{ // w œrodek - d³u¿szy
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 10, test2.c_str(), 15);
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2.substr(0, 15) + test1.substr(20), tString(b.getString()) );
		}
		{ // na œrodek - zerowa wstawka
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2 + test1.substr(10), tString(b.getString()) );
		}
		{ // na œrodek - zerowa podmianka
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 10, test2.c_str(), 0);
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test1.substr(20), tString(b.getString()) );
		}
		{ // na œrodek - zerowo generalnie
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 0, test2.c_str(), 0);
			CPPUNIT_ASSERT_EQUAL( test1, tString(b.getString()) );
		}
		{ // na koñcu
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(test1.size(), 10, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1 + test2, tString(b.getString()) );
		}
		{ // na koñcu - za daleko
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(200, 0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1 + test2, tString(b.getString()) );
		}
		{ // na œrodku - za d³ugi
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, Buffer<CHAR>::lengthUnknown, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2, tString(b.getString()) );
		}
		{ // pos + count == length
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, test1.size() - 10, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2, tString(b.getString()) );
		}
		{ // pos + count == length
			Buffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(5, 5, test2.c_str(), 5);
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 5) + test2.substr(0, 5) + test1.substr(10), tString(b.getString()) );
		}
		{ // referencja
			Buffer<CHAR> b;
			b.assignCheapReference(test1.c_str(), test1.size());
			b.replace(5, 5, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 5) + test2 + test1.substr(10), tString(b.getString()) );
			CPPUNIT_ASSERT( b.getBuffer() != test1.c_str() );
		}
		{ // referencja - bez danych po
			Buffer<CHAR> b;
			b.assignCheapReference(test1.c_str(), test1.size());
			b.replace(5, -1, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 5) + test2, tString(b.getString()) );
		}
		{ // referencja - 0 count
			Buffer<CHAR> b;
			b.assignCheapReference(test1.c_str(), test1.size());
			b.replace(5, 0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 5) + test2 + test1.substr(5), tString(b.getString()) );
		}
	}

	void testErase() {
		tString test = shortString();
		{
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(5, 10);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test.substr(15), tString(b.getString()) );
		}
		{ // usuwamy za du¿o
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(5, test.size());
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5), tString(b.getString()) );
		}
		{ // usuwamy 0
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(5, 0);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // czyscimy doszczetnie
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(0, test.size());
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // kasujemy poczatek
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(0, 10);
			CPPUNIT_ASSERT_EQUAL( test.substr(10), tString(b.getString()) );
		}
		{ // kasujemy z referencji
			Buffer<CHAR> b;
			b.assignCheapReference(test.c_str(), test.size());
			b.erase(5, 10);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test.substr(15), tString(b.getString()) );
		}
		{ // kasujemy z pustej referencji
			Buffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.erase(0, 10);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // kasujemy z pustego
			Buffer<CHAR> b;
			b.erase(0, 10);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}

	}

	void testTruncate() {
		tString test = shortString();
		{
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.truncate(5);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5), tString(b.getString()) );
		}
		{ // kasujemy poza
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.truncate(200);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // kasujemy do zera
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.truncate(0);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // kasujemy z referencji
			Buffer<CHAR> b;
			b.assignCheapReference(test.c_str(), test.size());
			b.truncate(5);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5), tString(b.getString()) );
		}
		{ // kasujemy z pustej referencji
			Buffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.truncate(0);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // kasujemy z pustego
			Buffer<CHAR> b;
			b.truncate(0);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
	}

	void testMakeUnique() {
		tString sRef = shortString();
		const CHAR* ref = sRef.c_str();
		Buffer<CHAR> b1;
		b1.assignCheapReference(ref);
		CPPUNIT_ASSERT( ref == b1.getBuffer() );
		b1.makeUnique();
		CPPUNIT_ASSERT( b1.getLength() == sRef.length() );
		CPPUNIT_ASSERT( ref != b1.getBuffer() );
		CPPUNIT_ASSERT_EQUAL( sRef, tString(b1.getBuffer()) );
		CPPUNIT_ASSERT( b1.isEmpty() == false );
		CPPUNIT_ASSERT( b1.isValid() == true );
		CPPUNIT_ASSERT( b1.isReference() == false );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == true );
		CPPUNIT_ASSERT_EQUAL( poolSmall, b1.getBufferSize() );
		CPPUNIT_ASSERT_EQUAL( shortString(), tString(ref) );
	}

	void testDiscard() {
		tString test = shortString();
		{
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.discard();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.isEmpty() == false );
			CHAR* buff = b.getBuffer();
			b.assign(test.c_str(), test.size());
			CPPUNIT_ASSERT( b.isValid() == true );
			CPPUNIT_ASSERT( b.isEmpty() == false );
			CPPUNIT_ASSERT( buff == b.getBuffer() );
		}
		{ // na pustym
			Buffer<CHAR> b;
			b.discard();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
			CPPUNIT_ASSERT( b.isEmpty() == true );
		}
		{ // na referencji
			Buffer<CHAR> b;
			b.assignCheapReference(test.c_str(), test.size());
			b.discard();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
			CPPUNIT_ASSERT( b.isEmpty() == true );
		}
	}

	void testReset() {
		tString test = shortString();
		{
			Buffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.reset();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
		}
		{ // na pustym
			Buffer<CHAR> b;
			b.reset();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
		}
		{ // na referencji
			Buffer<CHAR> b;
			b.assignCheapReference(test.c_str(), test.size());
			b.reset();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
		}
	}

	void testSwap() {
		tString t1 = shortString();
		tString t2 = mediumString();
		return;
		{
			Buffer<CHAR> a;
			Buffer<CHAR> b;
			a.assignCheapReference(t1.c_str(), t1.size());
			b.assign(t2.c_str(), t2.size());
			CHAR* b1 = a.getBuffer();
			CHAR* b2 = b.getBuffer();
			CPPUNIT_ASSERT( a.isReference() == true );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == true );
			a.swap(b);
			CPPUNIT_ASSERT( b.isReference() == true );
			CPPUNIT_ASSERT( a.hasOwnBuffer() == true );
			CPPUNIT_ASSERT( b.getBuffer() == b1 );
			CPPUNIT_ASSERT( a.getBuffer() == b2 );
		}
	}


	void testBufferRef() {
		{ // szybka konwersja
			tString txt = mediumString();
			tBuffer test(txt.c_str());
			tBuffer::BufferRef r(test);
			CPPUNIT_ASSERT_EQUAL( mediumString(), tString(r.getString()) );
			CPPUNIT_ASSERT( r.getBuffer() == test.getBuffer() );
			r.makeUnique();
			CPPUNIT_ASSERT_EQUAL( mediumString(), tString(r.getString()) );
			CPPUNIT_ASSERT( r.getBuffer() != test.getBuffer() );
		}
	}

	typename tBuffer::PassBuffer testPassRef1() {
		tString txt = mediumString();
		tBuffer b(txt.c_str());
		b.makeUnique();
		return b;
	}

	void testPassBuffer() {
		{
			tString as = shortString();
			tString bs = mediumString();
			tBuffer a(as.c_str());
			tBuffer b(bs.c_str());
			a = tBuffer::PassBuffer(b);
			CPPUNIT_ASSERT_EQUAL( mediumString(), tString(a.getString()) );
			CPPUNIT_ASSERT( b.getLength() == 0 );
		}
		{
			tString txt = mediumString();
			tBuffer a = tBuffer::PassBuffer(tBuffer( txt.c_str() ));
			CPPUNIT_ASSERT( a.getBuffer() == txt.c_str() );
		}
		{
			tBuffer a = testPassRef1();
			CPPUNIT_ASSERT_EQUAL( mediumString(), tString(a.getString()) );
		}
	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestStringBuffer<char> );
CPPUNIT_TEST_SUITE_REGISTRATION( TestStringBuffer<wchar_t> );

