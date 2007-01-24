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
		a = 0xcccccc;
	}

	bool operator == (const TestObj& b) {
		return a == b.a;
	}

	bool operator != (const TestObj& b) {
		return a != b.a;
	}

	int a;

	static int instances;

};

int TestObj::instances = 0;

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

	CPPUNIT_TEST( testEqual );
	CPPUNIT_TEST( testCompare );

	CPPUNIT_TEST( testResize );
	CPPUNIT_TEST( testMoveLeft );
	CPPUNIT_TEST( testMoveRight );

	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testAppend );
	CPPUNIT_TEST( testPrepend );
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testErase );
	CPPUNIT_TEST( testReplace );

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

	static const unsigned int poolSmall = 16;
	static const unsigned int poolMedium = 64;

	static const int standardCount = 50;


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

	void testEqual() {
		{
			tBuffer b = build();

			CPPUNIT_ASSERT( b.equal( build() ) == true );
			CPPUNIT_ASSERT( b.equal( build(standardCount * 2) ) == false );
			CPPUNIT_ASSERT( b.equal( build(standardCount / 2) ) == false );
			CPPUNIT_ASSERT( b.equal( build(0) ) == false );
			CPPUNIT_ASSERT( build(0).equal( b ) == false );

			CPPUNIT_ASSERT( b.equal( build(standardCount * 2), standardCount ) == true );
			CPPUNIT_ASSERT( b.equal( build(), standardCount * 2 ) == true );
			CPPUNIT_ASSERT( b.equal( build(), standardCount / 2 ) == true );
			CPPUNIT_ASSERT( b.equal( build(standardCount, 10), standardCount / 2 ) == false );

			b.getBuffer()[5].a = 200;
			CPPUNIT_ASSERT( b.equal( build() ) == false );
			CPPUNIT_ASSERT( TestObj::instances == standardCount );
		}
	}

	void testCompare() {
		{
			tBuffer b = build();

			CPPUNIT_ASSERT( b.compare( build() ) == 0 );
			CPPUNIT_ASSERT( b.compare( build(standardCount * 2) ) != 0 );
			CPPUNIT_ASSERT( b.compare( build(standardCount / 2) ) != 0 );
			CPPUNIT_ASSERT( b.compare( build(0) ) != 0 );
			CPPUNIT_ASSERT( build(0).compare( b ) != 0 );
			b.getBuffer()[5].a = 200;
			CPPUNIT_ASSERT( b.compare( build() ) != 0 );
			CPPUNIT_ASSERT( TestObj::instances == standardCount );
		}
	}


	void testResize() {
		// na czystym buforze - sprawdzamy czy go nie zvaliduje...
		{
		tBuffer b1;
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
		tBuffer b;
		b.assign( build() );

		CPPUNIT_ASSERT( TestObj::instances == standardCount );

		CPPUNIT_ASSERT( b.isValid() == true );
		b.resize(100);
		CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT_EQUAL( (unsigned)standardCount, b.getLength() );
		CPPUNIT_ASSERT( build() == b );
		b.resize(1000);
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
		CPPUNIT_ASSERT_EQUAL( (unsigned)standardCount, b.getLength() );
		CPPUNIT_ASSERT( build() == b );
		b.resize(5);
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT_EQUAL( 5, TestObj::instances );
		CPPUNIT_ASSERT_EQUAL( (unsigned)5, b.getLength() );
		CPPUNIT_ASSERT( build(5) == b );
		}

		// na pe³nym buforze - sprawdzamy szcz¹tkowe kopiowanie
		{
		tBuffer b = build();
		CPPUNIT_ASSERT( TestObj::instances == standardCount );
		b.resize(100, standardCount / 2);
		CPPUNIT_ASSERT( b.isValid() == true );
		CPPUNIT_ASSERT( TestObj::instances == standardCount / 2 );
		CPPUNIT_ASSERT_EQUAL( (unsigned)(standardCount / 2), b.getLength() );
		CPPUNIT_ASSERT( b == build(standardCount / 2) );
		}
	}






	void testMoveLeft(bool byRef) {
		tBuffer test = build();
		CPPUNIT_ASSERT( TestObj::instances == standardCount );
		{ // przesuwamy ca³oœæ od pocz¹tku
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, 5);
			//0123456789
			//56789
			CPPUNIT_ASSERT_EQUAL( standardCount + standardCount - 5, TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount - 5, 5) );
		}
		{ // przesuwamy kawa³ek od pocz¹tku - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, 2, 5, false);
			//0123456789
			//2343456789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 2, b.getBuffer()[0].a );
			CPPUNIT_ASSERT_EQUAL( 4, b.getBuffer()[2].a );
			CPPUNIT_ASSERT_EQUAL( 3, b.getBuffer()[3].a );
			CPPUNIT_ASSERT_EQUAL( 9, b.getBuffer()[9].a );
		}
		{ // przesuwamy ca³oœæ od pocz¹tku - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, 5, tBuffer::wholeData, false);
			//0123456789
			//5678956789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 9, b.getBuffer()[4].a );
			CPPUNIT_ASSERT_EQUAL( standardCount - 5, b.getBuffer()[standardCount - 5].a );
		}
		{ // przesuwamy kawa³ek od pocz¹tku - z truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, 2, 5, true);
			//0123456789
			//234
			CPPUNIT_ASSERT_EQUAL( standardCount + 3, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 4, b.getBuffer()[2].a );
			CPPUNIT_ASSERT_EQUAL( 2, b.getBuffer()[0].a );
		}
		{ // przesuwamy ca³oœæ zostawiaj¹c dwie litery na pocz¹tku
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(7, 5);
			//0123456789
			//01789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 - 5, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 1, b.getBuffer()[1].a );
			CPPUNIT_ASSERT_EQUAL( 7, b.getBuffer()[2].a );
		}
		{ // przesuwamy kilka w œrodku - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(5, 3, 3, false);
			// 0123456789
			// 0156756789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 1, b.getBuffer()[1].a );
			CPPUNIT_ASSERT_EQUAL( 5, b.getBuffer()[2].a );
			CPPUNIT_ASSERT_EQUAL( 7, b.getBuffer()[4].a );
			CPPUNIT_ASSERT_EQUAL( 5, b.getBuffer()[5].a );
		}
		{ // przesuwamy jeden w œrodku - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(2, 1, 1, false);
			// 0123456789
			// 0223456789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[0].a );
			CPPUNIT_ASSERT_EQUAL( 2, b.getBuffer()[1].a );
			CPPUNIT_ASSERT_EQUAL( 2, b.getBuffer()[2].a );
		}
		{ // przesuwamy poza zakres
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, standardCount + 2);
			CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( (unsigned int)0, b.getLength() );
		}
		{ // przesuwamy poza zakres zbyt krotkim ciagiem bez truncate - nic siê nie powinno zmienic
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, 10, 5, false);
			CPPUNIT_ASSERT( b == test );
			if (byRef) { // jako ¿e nic siê nie zmieni³o - nie zosta³ wykonany unique...
				CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
			} else {
				CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			}
		}
		{ // przesuwamy poza zakres zbyt krotkim ciagiem z truncate - powinien sie wyczyscic
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, 10, 5, true);
			CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( (unsigned int)0, b.getLength() );
		}
		{ // przesuwamy, ale zero znaków - z truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, 10, 0, true);
			CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( (unsigned int)0, b.getLength() );
		}
		{ // przesuwamy ze œrodka, ale zero znaków - z truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(7, 5, 0, true);
			CPPUNIT_ASSERT( b == build(2) );
			CPPUNIT_ASSERT_EQUAL( standardCount + 2, TestObj::instances );
		}
		{ // przesuwamy, ale zero znaków - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, 10, 0, false);
			CPPUNIT_ASSERT( b == test );
			if (byRef) { // jako ¿e nic siê nie zmieni³o - nie zosta³ wykonany unique...
				CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
			} else {
				CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			}
		}		
		{ // nie ruszamy
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(5, 0);
			CPPUNIT_ASSERT( b == test );
			if (byRef) { // jako ¿e nic siê nie zmieni³o - nie zosta³ wykonany unique...
				CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
			} else {
				CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			}
		}
		{ // przesuwamy za d³ugi tekst
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(5, 5, standardCount*2);
			//0123456789
			//56789
			CPPUNIT_ASSERT( b == build(standardCount - 5, 5) );
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 - 5, TestObj::instances );
		}
	}

	void testMoveLeft() {
		std::cout << " NoRef ";
		testMoveLeft(false);
		std::cout << " Ref ";
		testMoveLeft(true);
	}

	void testMoveRight(bool byRef) {
		tBuffer test = build();
		CPPUNIT_ASSERT( TestObj::instances == standardCount );
		{ // przesuwamy ca³y tekst od pocz¹tku
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(0, 5);
			//0123456789
			//012340123456789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 + 5, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[0].a );
			CPPUNIT_ASSERT_EQUAL( 4, b.getBuffer()[4].a );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[5].a );
			CPPUNIT_ASSERT_EQUAL( 5, b.getBuffer()[10].a );
		}
		{ // przesuwamy kawa³ek od pocz¹tku - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(0, 2, 5, false);
			//0123456789
			//0101234789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[0].a );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[2].a );
			CPPUNIT_ASSERT_EQUAL( 2, b.getBuffer()[4].a );
			CPPUNIT_ASSERT_EQUAL( 7, b.getBuffer()[7].a );
		}
		{ // przesuwamy ca³y od pocz¹tku - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(0, 5, Buffer<CHAR>::wholeData, false);
			//0123456789
			//012340123456789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 + 5, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[0].a );
			CPPUNIT_ASSERT_EQUAL( 4, b.getBuffer()[4].a );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[5].a );
			CPPUNIT_ASSERT_EQUAL( 5, b.getBuffer()[10].a );
		}
		{ // przesuwamy kawa³ek tekstu od pocz¹tku - z truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			//0123456789
			//0101234
			b.moveRight(0, 2, 5, true);
			CPPUNIT_ASSERT_EQUAL( standardCount + 7, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[0].a );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[2].a );
			CPPUNIT_ASSERT_EQUAL( 4, b.getBuffer()[6].a );
		}
		{ // przesuwamy ca³y zostawiaj¹c dwa na koñcu
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(0, 5, standardCount - 7, false);
			//0123456789
			//0123401289
			CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[0].a );
			CPPUNIT_ASSERT_EQUAL( 4, b.getBuffer()[4].a );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[5].a );
			CPPUNIT_ASSERT_EQUAL( 2, b.getBuffer()[7].a );
			CPPUNIT_ASSERT_EQUAL( standardCount - 2, b.getBuffer()[standardCount - 2].a );
		}
		{ // przesuwamy kilka liter w œrodku - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(standardCount - 6, 5, 5, false);
			//0123456789
			//01234567845678
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 + 4, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[0].a );
			CPPUNIT_ASSERT_EQUAL( standardCount - 2, b.getBuffer()[standardCount - 2].a );
			CPPUNIT_ASSERT_EQUAL( standardCount - 6, b.getBuffer()[standardCount - 1].a );
			CPPUNIT_ASSERT_EQUAL( standardCount - 5, b.getBuffer()[standardCount].a );
		}
		{ // przesuwamy jedn¹ literê w œrodku - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(1, 1, 1, false);
			//0123456789
			//0113456789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 1, b.getBuffer()[1].a );
			CPPUNIT_ASSERT_EQUAL( 1, b.getBuffer()[2].a );
			CPPUNIT_ASSERT_EQUAL( 3, b.getBuffer()[3].a );
			CPPUNIT_ASSERT_EQUAL( 5, b.getBuffer()[5].a );
		}
		{ // przesuwamy poza zakres
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(0, standardCount);
			//0123456789
			//01234567890123456789
			CPPUNIT_ASSERT_EQUAL( standardCount * 3, TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[0].a );
			CPPUNIT_ASSERT_EQUAL( 9, b.getBuffer()[9].a );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[standardCount].a );
		}
		{ // przesuwamy poza zakres krotkim ciagiem
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(0, standardCount, 5);
			//0123456789
			//012345678901234
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 + 5, TestObj::instances );
			CPPUNIT_ASSERT( b.equal(test, standardCount) );
			CPPUNIT_ASSERT_EQUAL( 0, b.getBuffer()[standardCount].a );
		}
		{ // przesuwamy, ale zero znaków - z truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(0, 5, 0, true);
			//0123456789
			//01234
			CPPUNIT_ASSERT_EQUAL( standardCount + 5, TestObj::instances );
			CPPUNIT_ASSERT( b.equal(build(5)) );
		}
		{ // przesuwamy ze œrodka, ale zero znaków - z truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(2, 5, 0, true);
			//0123456789
			CPPUNIT_ASSERT_EQUAL( standardCount + 7, TestObj::instances );
			CPPUNIT_ASSERT( b.equal(build(7)) );
		}
		{ // przesuwamy, ale zero znaków - bez truncate
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveLeft(0, 10, 0, false);
			//0123456789
			if (byRef) { // jako ¿e nic siê nie zmieni³o - nie zosta³ wykonany unique...
				CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
			} else {
				CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			}
			CPPUNIT_ASSERT( b.equal(test) );
		}		
		{ // nie ruszamy
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(5, 0);
			if (byRef) { // jako ¿e nic siê nie zmieni³o - nie zosta³ wykonany unique...
				CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
			} else {
				CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
			}
			CPPUNIT_ASSERT( b.equal(test) );
		}
		{ // przesuwamy za d³ugi tekst
			tBuffer b;
			if (byRef) b.assignCheapReference(test); else b.assign(test);
			b.moveRight(standardCount - 5, 5, standardCount*2);
			//0123456789
			//012345678956789
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 + 5 , TestObj::instances );
			CPPUNIT_ASSERT_EQUAL( standardCount - 1, b.getBuffer()[standardCount - 1].a );
			CPPUNIT_ASSERT_EQUAL( standardCount - 5, b.getBuffer()[standardCount].a );
		}

	}

	void testMoveRight() {
		std::cout << " NoRef ";
		testMoveRight(false);
		std::cout << " Ref ";
		testMoveRight(true);
	}


	void testAssign() {
		// clean - short
		{
		tBuffer b1;
		b1.assign(build(10));
		CPPUNIT_ASSERT_EQUAL( 10 , TestObj::instances );
		CPPUNIT_ASSERT( b1.isEmpty() == false );
		CPPUNIT_ASSERT( b1.isValid() == true );
		CPPUNIT_ASSERT( b1.isReference() == false );
		CPPUNIT_ASSERT( b1.hasOwnBuffer() == true );
		CPPUNIT_ASSERT( b1.getBufferSize() == poolSmall );
		CPPUNIT_ASSERT( b1.getLength() == 10 );
		CPPUNIT_ASSERT( b1 == build(10) );
		}
		// reuse - short
		{
		tBuffer b2(poolSmall);
		b2.assign( build(poolSmall) );
		CPPUNIT_ASSERT_EQUAL( (int)poolSmall , TestObj::instances );
		CPPUNIT_ASSERT( b2.isValid() == true );
		CPPUNIT_ASSERT( b2.getBufferSize() == poolSmall );
		CPPUNIT_ASSERT( b2.getLength() == poolSmall );
		CPPUNIT_ASSERT( b2 == build(poolSmall) );
		}

		// clean - medium
		{
		tBuffer b3;
		b3.assign( build(poolMedium) );
		CPPUNIT_ASSERT_EQUAL( (int)poolMedium , TestObj::instances );
		CPPUNIT_ASSERT_EQUAL( poolMedium, b3.getBufferSize() );
		CPPUNIT_ASSERT_EQUAL( poolMedium, b3.getLength() );
		CPPUNIT_ASSERT( b3 == build(poolMedium) );
		}

		// clean - large
		{
		tBuffer b4;
		b4.assign( build(600) );
		CPPUNIT_ASSERT_EQUAL( 600 , TestObj::instances );
		CPPUNIT_ASSERT( b4.getBufferSize() >= 600 );
		CPPUNIT_ASSERT_EQUAL( (unsigned)600, b4.getLength() );
		CPPUNIT_ASSERT( b4 == build(600) );
		}

		// reuse - large
		{
		tBuffer b5(5000);
		b5.assign( build(600) );
		CPPUNIT_ASSERT_EQUAL( 600 , TestObj::instances );
		CPPUNIT_ASSERT_EQUAL( (unsigned int)5000, b5.getBufferSize() );
		CPPUNIT_ASSERT_EQUAL( (unsigned)600, b5.getLength() );
		CPPUNIT_ASSERT( b5 == build(600) );
		}
	}



	void testAppend() {
		{
			tBuffer b (build());
			b.append(build(standardCount, standardCount));
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount*2) );
		}
		{ // do pustego
			tBuffer b;
			b.append(build());
			CPPUNIT_ASSERT_EQUAL( standardCount , TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
		{ // do pustej referencji
			tBuffer b;
			b.assignCheapReference(build().getBuffer(), 0);
			b.append(build());
			CPPUNIT_ASSERT_EQUAL( standardCount , TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
		{ // do referencji
			tBuffer b;
			{
				tBuffer ref (build());
				b.assignCheapReference(ref);
				b.append(build(standardCount, standardCount));
			}
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount*2) );
		}
		{ // dodajemy puste
			tBuffer b (build());
			b.append(build().getBuffer(), 0);
			CPPUNIT_ASSERT_EQUAL( standardCount , TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
	}

	void testPrepend() {
		{
			tBuffer b (build(standardCount, standardCount));
			b.prepend(build());
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount*2) );
		}
		{ // do pustego
			tBuffer b;
			b.prepend(build());
			CPPUNIT_ASSERT_EQUAL( standardCount  , TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
		{ // do pustej referencji
			tBuffer b;
			b.assignCheapReference(build().getBuffer(), 0);
			b.prepend(build());
			CPPUNIT_ASSERT_EQUAL( standardCount, TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
		{ // dodajemy puste
			tBuffer b (build());
			b.prepend(build().getBuffer(), 0);
			CPPUNIT_ASSERT_EQUAL( standardCount  , TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
	}

	void testInsert() {
		{ // na pocz¹tek
			tBuffer b (build(standardCount, standardCount));
			b.insert(0, build());
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount*2) );
		}
		{ // w œrodek
			tBuffer b (build());
			b.insert(5, build(standardCount, 5));
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			CPPUNIT_ASSERT( b.equal(build(standardCount), standardCount) );
			CPPUNIT_ASSERT_EQUAL( 5, b.getBuffer()[standardCount + 5].a);
		}
		{ // na koniec
			tBuffer b (build());
			b.insert(b.getLength(), build(standardCount, standardCount));
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount*2) );
		}
		{ // do pustego na pocz¹tek
			tBuffer b;
			b.insert(0, build());
			CPPUNIT_ASSERT_EQUAL( standardCount  , TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
		{ // dodajemy puste
			tBuffer b (build());
			b.insert(10, build().getBuffer(), 0);
			CPPUNIT_ASSERT_EQUAL( standardCount  , TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
		{ // wstawiamy poza - niebezpieczne!
			tBuffer b (build());
			b.insert(standardCount * 2, build());
			// mnóstwo elementów pomiêdzy nie zosta³o zainicjowanych, ale zostan¹ zniszczone!
			CPPUNIT_ASSERT_EQUAL( (unsigned)standardCount * 3, b.getLength() );
			CPPUNIT_ASSERT_EQUAL( standardCount * 2, TestObj::instances );
		}
		CPPUNIT_ASSERT_EQUAL( -standardCount, TestObj::instances );
		TestObj::instances = 0;
		{ // do pustej referencji
			tBuffer b;
			b.assignCheapReference(build().getBuffer(), 0);
			b.insert(0, build());
			CPPUNIT_ASSERT_EQUAL( standardCount  , TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
	}

	void testReplace() {
		{ // na pocz¹tku - zerowy
			tBuffer b (build(standardCount, standardCount));
			b.replace(0, 0, build());
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount*2) );
		}
		{ // na pocz¹tku
			tBuffer b (build());
			b.replace(0, 5, build());
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 - 5, TestObj::instances );
			tBuffer test (build());
			test.append(build(standardCount - 5, 5));
			CPPUNIT_ASSERT( b == test );
		}
		{ // w œrodek - krótszy
			tBuffer b (build());
			b.replace(5, 10, build(5));
			CPPUNIT_ASSERT_EQUAL( standardCount - 10 + 5 , TestObj::instances );
			tBuffer test (build(5));
			test.append(build(5));
			test.append(build(standardCount - 15, 15));
			CPPUNIT_ASSERT( b == test );
		}
		{ // w œrodek - równy
			tBuffer b (build());
			b.replace(5, 10, build(10));
			CPPUNIT_ASSERT_EQUAL( standardCount , TestObj::instances );
			tBuffer test (build(5));
			test.append(build(10));
			test.append(build(standardCount - 15, 15));
			CPPUNIT_ASSERT( b == test );
		}
		{ // w œrodek - d³u¿szy
			tBuffer b (build());
			b.replace(5, 10, build(15));
			CPPUNIT_ASSERT_EQUAL( standardCount - 10 + 15 , TestObj::instances );
			tBuffer test (build(5));
			test.append(build(15));
			test.append(build(standardCount - 15, 15));
			CPPUNIT_ASSERT( b == test );
		}
		{ // na œrodek - zerowa wstawka
			tBuffer b (build());
			b.replace(5, 0, build());
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			tBuffer test (build(5));
			test.append(build());
			test.append(build(standardCount - 5, 5));
			CPPUNIT_ASSERT( b == test );
		}
		{ // na œrodek - zerowa podmianka
			tBuffer b (build());
			b.replace(5, 10, build().getBuffer(), 0);
			CPPUNIT_ASSERT_EQUAL( standardCount - 10 , TestObj::instances );
			tBuffer test (build(5));
			test.append(build(standardCount - 15, 15));
			CPPUNIT_ASSERT( b == test );
		}
		{ // na koñcu
			tBuffer b (build());
			b.replace(standardCount, 10, build(standardCount, standardCount));
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount*2) );
		}
		{ // na koñcu - za daleko
			tBuffer b(build());
			b.replace(standardCount * 2, 0, build(standardCount, standardCount));
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount*2) );
		}
		{ // na œrodku - za d³ugi
			tBuffer b (build());
			b.replace(5, tBuffer::lengthUnknown, build());
			CPPUNIT_ASSERT_EQUAL( standardCount + 5 , TestObj::instances );
			tBuffer test (build(5));
			test.append(build());
			CPPUNIT_ASSERT( b == test );
		}
		{ // pos + count == length
			tBuffer b (build());
			b.replace(5, standardCount - 5, build());
			CPPUNIT_ASSERT_EQUAL( standardCount + 5 , TestObj::instances );
			tBuffer test (build(5));
			test.append(build());
			CPPUNIT_ASSERT( b == test );
		}
		{ // pos + count == length
			tBuffer b (build());
			b.replace(5, 5, build(5));
			CPPUNIT_ASSERT_EQUAL( standardCount  , TestObj::instances );
			tBuffer test (build(5));
			test.append(build(5));
			test.append(build(standardCount - 10, 10));
			CPPUNIT_ASSERT( b == test );
		}
		{ // referencja
			tBuffer b;
			tBuffer ref(build());
			b.assignCheapReference(ref);
			b.replace(5, 5, build());
			CPPUNIT_ASSERT_EQUAL( standardCount * 3 - 5, TestObj::instances );
			tBuffer test (build(5));
			test.append(build());
			test.append(build(standardCount - 10, 10));
			CPPUNIT_ASSERT( b == test );
			CPPUNIT_ASSERT( b.getBuffer() != ref.getBuffer() );
		}
		{ // referencja - bez danych po
			tBuffer b;
			tBuffer ref(build());
			b.assignCheapReference(ref);
			b.replace(5, -1, build());
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 + 5 , TestObj::instances );
			tBuffer test (build(5));
			test.append(build());
			CPPUNIT_ASSERT( b == test );
		}
		{ // referencja - 0 count
			tBuffer b;
			tBuffer ref(build());
			b.assignCheapReference(ref);
			b.replace(5, 0, build());
			CPPUNIT_ASSERT_EQUAL( standardCount * 3, TestObj::instances );
			tBuffer test (build(5));
			test.append(build());
			test.append(build(standardCount - 5, 5));
			CPPUNIT_ASSERT( b == test );
		}
	}

	void testErase() {
		{
			tBuffer b (build());
			b.erase(5, 10);
			CPPUNIT_ASSERT_EQUAL( standardCount - 10 , TestObj::instances );
			tBuffer test (build(5));
			test.append(build(standardCount - 15, 15));
			CPPUNIT_ASSERT( b == test );
		}
		{ // usuwamy za du¿o
			tBuffer b (build());
			b.erase(5, b.getLength());
			CPPUNIT_ASSERT_EQUAL( 5 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(5) );
		}
		{ // usuwamy 0
			tBuffer b (build());
			b.erase(5, 0);
			CPPUNIT_ASSERT_EQUAL( standardCount , TestObj::instances );
			CPPUNIT_ASSERT( b == build() );
		}
		{ // czyscimy doszczetnie
			tBuffer b (build());
			b.erase(0, b.getLength());
			CPPUNIT_ASSERT_EQUAL( 0 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(0) );
		}
		{ // kasujemy poczatek
			tBuffer b (build());
			b.erase(0, 5);
			CPPUNIT_ASSERT_EQUAL( standardCount - 5 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(standardCount - 5, 5) );
		}
		{ // kasujemy z referencji
			tBuffer b;
			tBuffer ref(build());
			b.assignCheapReference(ref);
			b.erase(5, 10);
			CPPUNIT_ASSERT_EQUAL( standardCount * 2 - 10 , TestObj::instances );
			tBuffer test (build(5));
			test.append(build(standardCount - 15, 15));
			CPPUNIT_ASSERT( b == test );
		}
		{ // kasujemy z pustej referencji
			tBuffer b;
			b.assignCheapReference(build().getBuffer(), 0);
			b.erase(0, 10);
			CPPUNIT_ASSERT_EQUAL( 0 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(0) );
		}
		{ // kasujemy z pustego
			tBuffer b;
			b.erase(0, 10);
			CPPUNIT_ASSERT_EQUAL( 0 , TestObj::instances );
			CPPUNIT_ASSERT( b == build(0) );
		}

	}





};

CPPUNIT_TEST_SUITE_REGISTRATION( TestBuffer );

