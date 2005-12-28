#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <Stamina/VersionControl.h>
#include <Stamina/Buffer.h>

using namespace Stamina;


class TestObj {
public:
	TestObj(int a):a(a) {
		instances ++;
	}
	TestObj(const TestObj& b) {
		a = b.a;
		instances ++;
	}
	~TestObj() {
		instances --;
	}

	int a;

	static unsigned int instances;

};

unsigned int TestObj::instances = 0;

class TestBuffer : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestBuffer );
  
	CPPUNIT_TEST( testConstruction );
	CPPUNIT_TEST( testCreation );
	CPPUNIT_TEST( testSwap );
	CPPUNIT_TEST( testPassBuffer );
	CPPUNIT_TEST( testCheapReference );
	CPPUNIT_TEST( testMakeUnique );
	CPPUNIT_TEST( testBufferRef );
/*	CPPUNIT_TEST( testAssign );
*/
/*	
	CPPUNIT_TEST( testResize );
	CPPUNIT_TEST( testMakeRoom );
	CPPUNIT_TEST( testMoveLeft );
	CPPUNIT_TEST( testMoveRight );
	CPPUNIT_TEST( testAppend );
	CPPUNIT_TEST( testPrepend );
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testErase );
	CPPUNIT_TEST( testTruncate );
	CPPUNIT_TEST( testDiscard );
	CPPUNIT_TEST( testReset );
	CPPUNIT_TEST( testReplace );
*/
	CPPUNIT_TEST_SUITE_END();

protected:

public:

	typedef Buffer<TestObj> tBuffer;
	typedef Buffer<TestObj*> tPBuffer;

	void setUp() {

	}
	void tearDown() {
	}

	tBuffer build(int count = standardCount, int start = 0) {
		tBuffer buff(count);
		while(count--) {
			TestObj obj(start++);
			buff.append(&obj, 1);
		}
		return tBuffer::PassBuffer(buff);
	}

	static const unsigned int poolSmall = 32;
	static const unsigned int poolMedium = 128;

	static const unsigned int standardCount = 10;


protected:

	void testConstruction() {

		{
		tBuffer b1;
		CPPUNIT_ASSERT( b1.isEmpty() == true );
		CPPUNIT_ASSERT( b1.isValid() == false );
		CPPUNIT_ASSERT( b1.isReference() == false );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == false );
		CPPUNIT_ASSERT( b1.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b1.getLength() == 0 );
		CPPUNIT_ASSERT( TestObj::instances == 0 );
		}
		{
		tBuffer b2(10);
		CPPUNIT_ASSERT( b2.isEmpty() == false );
		CPPUNIT_ASSERT( b2.isValid() == false );
		CPPUNIT_ASSERT( b2.isReference() == false );
		CPPUNIT_ASSERT( b2.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b2.getBufferSize() == 16 );
		CPPUNIT_ASSERT( b2.getLength() == 0 );
		CPPUNIT_ASSERT( TestObj::instances == 0 );
		}
		{
		tBuffer b3(60);
		CPPUNIT_ASSERT( b3.isEmpty() == false );
		CPPUNIT_ASSERT( b3.isValid() == false );
		CPPUNIT_ASSERT( b3.isReference() == false );
		CPPUNIT_ASSERT( b3.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b3.getBufferSize() == 64 );
		CPPUNIT_ASSERT( b3.getLength() == 0 );
		CPPUNIT_ASSERT( TestObj::instances == 0 );
		}
		{
		tBuffer b4(1025);
		CPPUNIT_ASSERT( b4.isEmpty() == false );
		CPPUNIT_ASSERT( b4.isValid() == false );
		CPPUNIT_ASSERT( b4.isReference() == false );
		CPPUNIT_ASSERT( b4.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b4.getBufferSize() == 1025 );
		CPPUNIT_ASSERT( b4.getLength() == 0 );
		CPPUNIT_ASSERT( TestObj::instances == 0 );
		}
	}


	void testCreation() {
		tBuffer buff;
        TestObj a(1);
		TestObj b(2);
		TestObj c(3);
		int instances = TestObj::instances;

		buff.append(&a, 1);
		buff.append(&b, 1);
		buff.append(&c, 1);

		CPPUNIT_ASSERT( buff.isValid() == true );
		CPPUNIT_ASSERT( buff.getLength() == 3 );

		TestObj* array = buff.getBuffer();
		CPPUNIT_ASSERT( array[0].a == a.a );
		CPPUNIT_ASSERT( array[1].a == b.a );
		CPPUNIT_ASSERT( array[2].a == c.a );
		CPPUNIT_ASSERT( TestObj::instances == instances * 2 );

		buff.reset();

		CPPUNIT_ASSERT( buff.getLength() == 0 );

		CPPUNIT_ASSERT( TestObj::instances == instances );
	}

	void testSwap() {
		tBuffer b1;
		tBuffer b2;
        TestObj x(1);
		TestObj y(2);
		int instances = TestObj::instances;

		b1.append(&x, 1);
		b1.append(&y, 1);
		
		CPPUNIT_ASSERT( b1.getLength() == 2 );
		CPPUNIT_ASSERT( b2.getLength() == 0 );

		b2.swap(b1);

		CPPUNIT_ASSERT( b1.getLength() == 0 );
		CPPUNIT_ASSERT( b2.getLength() == 2 );
		CPPUNIT_ASSERT( b2.getBuffer()[0].a == x.a );
		CPPUNIT_ASSERT( b2.getBuffer()[1].a == y.a );

		CPPUNIT_ASSERT( TestObj::instances == instances * 2 );
	}

	void testPassBuffer() {
		tBuffer b1;
        TestObj x(1);
		int instances = TestObj::instances;

		b1.append(&x, 1);
		b1.append(&x, 1);

		CPPUNIT_ASSERT( b1.getLength() == 2 );

		tBuffer b2 = tBuffer::PassBuffer( b1 );

		CPPUNIT_ASSERT( b1.getLength() == 0 );
		CPPUNIT_ASSERT( b2.getLength() == 2 );
		CPPUNIT_ASSERT( b2.getBuffer()[0].a == x.a );
		CPPUNIT_ASSERT( TestObj::instances == instances * 3 );
	}



	void testCheapReference() {
		tBuffer bRef = build(standardCount);
		const TestObj* ref = bRef.getBuffer();

		CPPUNIT_ASSERT( TestObj::instances == bRef.getLength() );

		// clean
		{
		tBuffer b1;
		b1.assignCheapReference(ref, bRef.getLength());
		CPPUNIT_ASSERT( b1.isEmpty() == false );
		CPPUNIT_ASSERT( b1.isValid() == true );
		CPPUNIT_ASSERT( b1.isReference() == true );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == false );
		CPPUNIT_ASSERT( b1.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b1.getLength() == bRef.getLength() );
		CPPUNIT_ASSERT_EQUAL( (TestObj*)ref, (TestObj*)b1.getBuffer() );
		CPPUNIT_ASSERT( TestObj::instances == bRef.getLength() );
		}
		// after buffer
		{
		tBuffer b2(128);
		b2.assignCheapReference(bRef);
		CPPUNIT_ASSERT( b2.isEmpty() == false );
		CPPUNIT_ASSERT( b2.isValid() == true );
		CPPUNIT_ASSERT( b2.isReference() == true );
		CPPUNIT_ASSERT( b2.hasOwnBuffer() == false );
		CPPUNIT_ASSERT( b2.getBufferSize() == 0 );
		CPPUNIT_ASSERT( b2.getLength() == bRef.getLength() );
		CPPUNIT_ASSERT_EQUAL( (TestObj*)ref, (TestObj*)b2.getBuffer() );
		}

		CPPUNIT_ASSERT( TestObj::instances == bRef.getLength() );

		CPPUNIT_ASSERT_EQUAL( ref[0].a, 0 );
		CPPUNIT_ASSERT_EQUAL( ref[standardCount - 1].a, (int)(standardCount - 1) );
	}

	void testMakeUnique() {
		tBuffer ref = build(standardCount);

		tBuffer buff;
		buff.assignCheapReference(ref);

		CPPUNIT_ASSERT( TestObj::instances == standardCount );
		CPPUNIT_ASSERT( ref.getBuffer() == buff.getBuffer() );

		buff.makeUnique();

		CPPUNIT_ASSERT( TestObj::instances == standardCount * 2 );

		CPPUNIT_ASSERT( buff.getLength() == ref.getLength() );
		CPPUNIT_ASSERT( ref.getBuffer() != buff.getBuffer() );
		CPPUNIT_ASSERT( buff.isEmpty() == false );
		CPPUNIT_ASSERT( buff.isValid() == true );
		CPPUNIT_ASSERT( buff.isReference() == false );
		CPPUNIT_ASSERT( buff.hasOwnBuffer() == true );
		CPPUNIT_ASSERT_EQUAL( ref.getBuffer()[0].a, 0 );
		CPPUNIT_ASSERT_EQUAL( ref.getBuffer()[standardCount - 1].a, (int)(standardCount - 1) );	
		CPPUNIT_ASSERT_EQUAL( buff.getBuffer()[0].a, 0 );
		CPPUNIT_ASSERT_EQUAL( buff.getBuffer()[standardCount - 1].a, (int)(standardCount - 1) );	
	}


	void testBufferRef() {
		{ // szybka konwersja
			tBuffer buff = build(standardCount);

			CPPUNIT_ASSERT( TestObj::instances == standardCount );

			tBuffer::BufferRef ref(buff);

			CPPUNIT_ASSERT_EQUAL( buff.getBuffer()[0].a, 0 );
			CPPUNIT_ASSERT_EQUAL( buff.getBuffer()[standardCount - 1].a, (int)(standardCount - 1) );	
			CPPUNIT_ASSERT( ref.getBuffer() == buff.getBuffer() );
			ref.makeUnique();
			CPPUNIT_ASSERT_EQUAL( ref.getBuffer()[0].a, 0 );
			CPPUNIT_ASSERT_EQUAL( ref.getBuffer()[standardCount - 1].a, (int)(standardCount - 1) );	
			CPPUNIT_ASSERT( TestObj::instances == standardCount * 2 );
		}
	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestBuffer );

