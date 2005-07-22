#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <Stamina/VersionControl.h>
#include <Stamina/WideChar.h>
#include <Stamina/StringBuffer.h>
#include <Stamina/String.h>
#include <ConvertUTF.h>

using namespace Stamina;

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
		std::string s;
		s.resize(30, 'a');
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tString mediumString() {
		std::string s;
		s.resize(70, 'b');
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tString longString() {
		std::string s;
		s.resize(500, 'c');
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
	}

	void testMoveLeft() {
	}

	void testMoveRight() {
	}

	void testAppend() {
	}

	void testPrepend() {
	}

	void testInsert() {
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
	}

	void testReset() {
	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestStringBuffer<char> );
//CPPUNIT_TEST_SUITE_REGISTRATION( TestStringBuffer<wchar_t> );

