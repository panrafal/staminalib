#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <Stamina/VersionControl.h>
#include <Stamina/WideChar.h>
#include <Stamina/StringBuffer.h>
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
	CPPUNIT_TEST( testActive );
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

	CPPUNIT_TEST_SUITE_END();

protected:

	typedef std::basic_string<CHAR> tString;

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
		StringBuffer<CHAR> b1;
		CPPUNIT_ASSERT( b1.isEmpty() == true );
		CPPUNIT_ASSERT( b1.isValid() == false );
		CPPUNIT_ASSERT( b1.isActive() == false );
		CPPUNIT_ASSERT( b1.isReference() == false );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == false );
		CPPUNIT_ASSERT( b1.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b1.getLength() == 0 );
		}
		{
		StringBuffer<CHAR> b2(10);
		CPPUNIT_ASSERT( b2.isEmpty() == false );
		CPPUNIT_ASSERT( b2.isValid() == false );
		CPPUNIT_ASSERT( b2.isActive() == false );
		CPPUNIT_ASSERT( b2.isReference() == false );
		CPPUNIT_ASSERT( b2.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b2.getBufferSize() == 32 );
		CPPUNIT_ASSERT( b2.getLength() == 0 );
		}
		{
		StringBuffer<CHAR> b3(60);
		CPPUNIT_ASSERT( b3.isEmpty() == false );
		CPPUNIT_ASSERT( b3.isValid() == false );
		CPPUNIT_ASSERT( b3.isActive() == false );
		CPPUNIT_ASSERT( b3.isReference() == false );
		CPPUNIT_ASSERT( b3.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b3.getBufferSize() == 128 );
		CPPUNIT_ASSERT( b3.getLength() == 0 );
		}
		{
		StringBuffer<CHAR> b4(1025);
		CPPUNIT_ASSERT( b4.isEmpty() == false );
		CPPUNIT_ASSERT( b4.isValid() == false );
		CPPUNIT_ASSERT( b4.isActive() == false );
		CPPUNIT_ASSERT( b4.isReference() == false );
		CPPUNIT_ASSERT( b4.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b4.getBufferSize() == 1025 );
		CPPUNIT_ASSERT( b4.getLength() == 0 );
		}
	}

	void testActive() {
		StringBuffer<CHAR> b1;
		CPPUNIT_ASSERT( b1.isActive() == false );
		CPPUNIT_ASSERT( b1.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b1.getLength() == 0 );
		b1.setActive(true);
		CPPUNIT_ASSERT( b1.isActive() == true );
		CPPUNIT_ASSERT( b1.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b1.getLength() == 0 );
	}

	void testCheapReference() {
		tString sRef = shortString();
		const CHAR* ref = sRef.c_str();

		// clean
		{
		StringBuffer<CHAR> b1;
		b1.assignCheapReference(ref);
		CPPUNIT_ASSERT( b1.isEmpty() == false );
		CPPUNIT_ASSERT( b1.isValid() == true );
		CPPUNIT_ASSERT( b1.isActive() == false );
		CPPUNIT_ASSERT( b1.isReference() == true );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == false );
		CPPUNIT_ASSERT( b1.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b1.getLength() == sRef.length() );
		CPPUNIT_ASSERT_EQUAL( (CHAR*)ref, (CHAR*)b1.getBuffer() );
		}
		// after buffer
		{
		StringBuffer<CHAR> b2(128);
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
		StringBuffer<CHAR> b1;
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
		StringBuffer<CHAR> b2(poolSmall);
		b2.assign(shortString().c_str(), shortString().size());
		CPPUNIT_ASSERT( b2.isValid() == true );
		CPPUNIT_ASSERT( b2.getBufferSize() == poolSmall );
		CPPUNIT_ASSERT( b2.getLength() == shortString().length() );
		CPPUNIT_ASSERT_EQUAL( shortString(), tString(b2.getBuffer()) );
		}

		// clean - medium
		{
		StringBuffer<CHAR> b3;
		b3.assign(mediumString().c_str(), mediumString().size());
		CPPUNIT_ASSERT_EQUAL( poolMedium, b3.getBufferSize() );
		CPPUNIT_ASSERT_EQUAL( mediumString().length(), b3.getLength() );
		CPPUNIT_ASSERT_EQUAL( mediumString(), tString(b3.getBuffer()) );
		}

		// clean - large
		{
		StringBuffer<CHAR> b4;
		b4.assign(longString().c_str(), longString().size());
		CPPUNIT_ASSERT( b4.getBufferSize() >= longString().length() );
		CPPUNIT_ASSERT_EQUAL( longString().length(), b4.getLength() );
		CPPUNIT_ASSERT_EQUAL( longString(), tString(b4.getBuffer()) );
		}

		// reuse - large
		{
		StringBuffer<CHAR> b5(5000);
		b5.assign(longString().c_str(), longString().size());
		CPPUNIT_ASSERT_EQUAL( (unsigned int)5000, b5.getBufferSize() );
		CPPUNIT_ASSERT_EQUAL( longString().length(), b5.getLength() );
		CPPUNIT_ASSERT_EQUAL( longString(), tString(b5.getBuffer()) );
		}
	}

	void testResize() {
		// na czystym buforze - sprawdzamy czy go nie zvaliduje...
		{
		StringBuffer<CHAR> b1;
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
		StringBuffer<CHAR> b;
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
		StringBuffer<CHAR> b;
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
		StringBuffer<CHAR> b;
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
			StringBuffer<CHAR> b;
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
			StringBuffer<CHAR> b;
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
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 5);
			CPPUNIT_ASSERT_EQUAL( test.substr(5), tString(b.getString()) );
		}
		{ // przesuwamy kawa³ek tekstu od pocz¹tku - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			// 0123456789012345678901234567890
			// 2343456789012345678901234567890
			b.moveLeft(0, 2, 5, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(2, 3) + test.substr(3), tString(b.getString()) );
		}
		{ // przesuwamy ca³y tekst od pocz¹tku - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 5, StringBuffer<CHAR>::wholeData, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(5) + test.substr(size - 5), tString(b.getString()) );
		}
		{ // przesuwamy kawa³ek tekstu od pocz¹tku - z truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			// 0123456789012345678901234567890
			// 2343456789012345678901234567890
			b.moveLeft(0, 2, 5, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(2, 3), tString(b.getString()) );
		}
		{ // przesuwamy ca³y tekst zostawiaj¹c dwie litery na pocz¹tku
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(7, 5);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2) + test.substr(7), tString(b.getString()) );
		}
		{ // przesuwamy kilka liter w œrodku - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(7, 5, 5, false);
			// 123456789012345678901234567890
			// 128901289012345678901234567890
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2) + test.substr(7, 5) + test.substr(7), tString(b.getString()) );
		}
		{ // przesuwamy jedn¹ literê w œrodku - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(1, 1, 1, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(1, 1) + test.substr(1, 1) + test.substr(2), tString(b.getString()) );
		}
		{ // przesuwamy poza zakres
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, size + 2);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // przesuwamy poza zakres zbyt krotkim ciagiem bez truncate - nic siê nie powinno zmienic
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 5, false);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // przesuwamy poza zakres zbyt krotkim ciagiem z truncate - powinien sie wyczyscic
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 5, true);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // przesuwamy, ale zero znaków - z truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 0, true);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // przesuwamy ze œrodka, ale zero znaków - z truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(12, 10, 0, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2), tString(b.getString()) );
		}
		{ // przesuwamy, ale zero znaków - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 0, false);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}		
		{ // nie ruszamy
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(5, 0);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // przesuwamy za d³ugi tekst
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(5, 10, size*2);
			CPPUNIT_ASSERT_EQUAL( test.substr(10), tString(b.getString()) );
		}
		{ // przesuwamy pust¹ referencjê
			StringBuffer<CHAR> b;
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
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, 5);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test, tString(b.getString()) );
		}
		{ // przesuwamy kawa³ek tekstu od pocz¹tku - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			// 0123456789012345678901234567890
			// 0101234789012345678901234567890
			b.moveRight(0, 2, 5, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2) + test.substr(0, 5) + test.substr(7), tString(b.getString()) );
		}
		{ // przesuwamy ca³y tekst od pocz¹tku - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, 5, StringBuffer<CHAR>::wholeData, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test, tString(b.getString()) );
		}
		{ // przesuwamy kawa³ek tekstu od pocz¹tku - z truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			// 0123456789012345678901234567890
			// 0101234
			b.moveRight(0, 2, 5, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 2) + test.substr(0, 5), tString(b.getString()) );
		}
		{ // przesuwamy ca³y tekst zostawiaj¹c dwie litery na koñcu
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, 5, size - 7, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test.substr(0, size - 7) + test.substr(size - 2, 2), tString(b.getString()) );
		}
		{ // przesuwamy kilka liter w œrodku - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(7, 5, 5, false);
			// 0123456789012345678901234567890
			// 0123456789017890178901234567890
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 12) + test.substr(7, 5) + test.substr(17), tString(b.getString()) );
		}
		{ // przesuwamy jedn¹ literê w œrodku - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(1, 1, 1, false);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 1) + test.substr(1, 1) + test.substr(1, 1) + test.substr(3), tString(b.getString()) );
		}
		{ // przesuwamy poza zakres
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, size);
			CPPUNIT_ASSERT_EQUAL( test + test, tString(b.getString()) );
		}
		{ // przesuwamy poza zakres krotkim ciagiem
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, size, 5);
			CPPUNIT_ASSERT_EQUAL( test + test.substr(0, 5), tString(b.getString()) );
		}
		{ // przesuwamy, ale zero znaków - z truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(0, 10, 0, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 10), tString(b.getString()) );
		}
		{ // przesuwamy ze œrodka, ale zero znaków - z truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(2, 10, 0, true);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 12), tString(b.getString()) );
		}
		{ // przesuwamy, ale zero znaków - bez truncate
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveLeft(0, 10, 0, false);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}		
		{ // nie ruszamy
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(5, 0);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // przesuwamy za d³ugi tekst
			StringBuffer<CHAR> b;
			if (byRef) b.assignCheapReference(testRef); else b.assign(testRef, size);
			b.moveRight(5, 10, size*2);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 15) + test.substr(5), tString(b.getString()) );
		}
		{ // przesuwamy pust¹ referencjê
			StringBuffer<CHAR> b;
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
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.append(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1 + test2, tString(b.getString()) );
		}
		{ // do pustego
 			StringBuffer<CHAR> b;
			b.append(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // do pustej referencji
			StringBuffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.append(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // dodajemy puste
			StringBuffer<CHAR> b; 
			b.assign(test1.c_str(), test1.size());
			b.append((CHAR*)L"", 0);
			CPPUNIT_ASSERT_EQUAL( test1, tString(b.getString()) );
		}
	}

	void testPrepend() {
		tString test1 = shortString();
		tString test2 = mediumString();
		{
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.prepend(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2 + test1, tString(b.getString()) );
		}
		{ // do pustego
			StringBuffer<CHAR> b;
			b.prepend(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // do pustej referencji
			StringBuffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.prepend(test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // dodajemy puste
			StringBuffer<CHAR> b; 
			b.assign(test1.c_str(), test1.size());
			b.prepend((CHAR*)L"", 0);
			CPPUNIT_ASSERT_EQUAL( test1, tString(b.getString()) );
		}
	}

	void testInsert() {
		tString test1 = shortString();
		tString test2 = mediumString();
		{ // na pocz¹tek
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.insert(0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2 + test1, tString(b.getString()) );
		}
		{ // w œrodek
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.insert(10, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2 + test1.substr(10), tString(b.getString()) );
		}
		{ // na koniec
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.insert(b.getLength(), test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1 + test2, tString(b.getString()) );
		}
		{ // do pustego na pocz¹tek
			StringBuffer<CHAR> b;
			b.insert(0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
		{ // dodajemy puste
			StringBuffer<CHAR> b; 
			b.assign(test1.c_str(), test1.size());
			b.insert(10, (CHAR*)L"", 0);
			CPPUNIT_ASSERT_EQUAL( test1, tString(b.getString()) );
		}
		{ // wstawiamy poza
			StringBuffer<CHAR> b; 
			b.assign(test1.c_str(), test1.size());
			b.insert(b.getLength() * 2, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.size()*2 + test2.size(), b.getLength() );
		}
		{ // do pustej referencji
			StringBuffer<CHAR> b; 
			b.assignCheapReference((CHAR*)L"", 0);
			b.insert(0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2, tString(b.getString()) );
		}
	}

	void testReplace() {
		tString test1 = mediumString();
		tString test2 = shortString();
		{ // na pocz¹tku - zerowy
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(0, 0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2 + test1, tString(b.getString()) );
		}
		{ // na pocz¹tku
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(0, 5, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test2 + test1.substr(5), tString(b.getString()) );
		}
		{ // w œrodek - krótszy
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 10, test2.c_str(), 5);
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2.substr(0,5) + test1.substr(20), tString(b.getString()) );
		}
		{ // w œrodek - równy
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 10, test2.c_str(), 10);
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2.substr(0,10) + test1.substr(20), tString(b.getString()) );
		}
		{ // w œrodek - d³u¿szy
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 10, test2.c_str(), 15);
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2.substr(0, 15) + test1.substr(20), tString(b.getString()) );
		}
		{ // na œrodek - zerowa wstawka
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test2 + test1.substr(10), tString(b.getString()) );
		}
		{ // na œrodek - zerowa podmianka
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 10, test2.c_str(), 0);
			CPPUNIT_ASSERT_EQUAL( test1.substr(0, 10) + test1.substr(20), tString(b.getString()) );
		}
		{ // na œrodek - zerowo generalnie
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(10, 0, test2.c_str(), 0);
			CPPUNIT_ASSERT_EQUAL( test1, tString(b.getString()) );
		}
		{ // na koñcu
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(test1.size(), 10, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1 + test2, tString(b.getString()) );
		}
		{ // na koñcu - za daleko
			StringBuffer<CHAR> b;
			b.assign(test1.c_str(), test1.size());
			b.replace(200, 0, test2.c_str(), test2.size());
			CPPUNIT_ASSERT_EQUAL( test1 + test2, tString(b.getString()) );
		}
	}

	void testErase() {
		tString test = shortString();
		{
			StringBuffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(5, 10);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test.substr(15), tString(b.getString()) );
		}
		{ // usuwamy za du¿o
			StringBuffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(5, test.size());
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5), tString(b.getString()) );
		}
		{ // usuwamy 0
			StringBuffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(5, 0);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // czyscimy doszczetnie
			StringBuffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(0, test.size());
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // kasujemy poczatek
			StringBuffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.erase(0, 10);
			CPPUNIT_ASSERT_EQUAL( test.substr(10), tString(b.getString()) );
		}
		{ // kasujemy z referencji
			StringBuffer<CHAR> b;
			b.assignCheapReference(test.c_str(), test.size());
			b.erase(5, 10);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5) + test.substr(15), tString(b.getString()) );
		}
		{ // kasujemy z pustej referencji
			StringBuffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.erase(0, 10);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // kasujemy z pustego
			StringBuffer<CHAR> b;
			b.erase(0, 10);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}

	}

	void testTruncate() {
		tString test = shortString();
		{
			StringBuffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.truncate(5);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5), tString(b.getString()) );
		}
		{ // kasujemy poza
			StringBuffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.truncate(200);
			CPPUNIT_ASSERT_EQUAL( test, tString(b.getString()) );
		}
		{ // kasujemy do zera
			StringBuffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.truncate(0);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // kasujemy z referencji
			StringBuffer<CHAR> b;
			b.assignCheapReference(test.c_str(), test.size());
			b.truncate(5);
			CPPUNIT_ASSERT_EQUAL( test.substr(0, 5), tString(b.getString()) );
		}
		{ // kasujemy z pustej referencji
			StringBuffer<CHAR> b;
			b.assignCheapReference((CHAR*)L"", 0);
			b.truncate(0);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
		{ // kasujemy z pustego
			StringBuffer<CHAR> b;
			b.truncate(0);
			CPPUNIT_ASSERT_EQUAL( tString(), tString(b.getString()) );
		}
	}

	void testMakeUnique() {
		tString sRef = shortString();
		const CHAR* ref = sRef.c_str();
		StringBuffer<CHAR> b1;
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
			StringBuffer<CHAR> b;
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
			StringBuffer<CHAR> b;
			b.discard();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
			CPPUNIT_ASSERT( b.isEmpty() == true );
		}
		{ // na referencji
			StringBuffer<CHAR> b;
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
			StringBuffer<CHAR> b;
			b.assign(test.c_str(), test.size());
			b.reset();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
		}
		{ // na pustym
			StringBuffer<CHAR> b;
			b.reset();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
		}
		{ // na referencji
			StringBuffer<CHAR> b;
			b.assignCheapReference(test.c_str(), test.size());
			b.reset();
			CPPUNIT_ASSERT( b.isValid() == false );
			CPPUNIT_ASSERT( b.hasOwnBuffer() == false );
		}
	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestStringBuffer<char> );
CPPUNIT_TEST_SUITE_REGISTRATION( TestStringBuffer<wchar_t> );

