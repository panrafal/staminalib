#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <Stamina/VersionControl.h>
#include <Stamina/Array.h>

using namespace Stamina;



class TestArray : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestArray );
  
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testErase );
	CPPUNIT_TEST_EXCEPTION( testEraseOutOfRange1, iArrayBase::ExceptionOutOfRange );
	CPPUNIT_TEST_EXCEPTION( testEraseOutOfRange2, iArrayBase::ExceptionOutOfRange );
	CPPUNIT_TEST_EXCEPTION( testOutOfRange1, iArrayBase::ExceptionOutOfRange );
	CPPUNIT_TEST_EXCEPTION( testOutOfRange2, iArrayBase::ExceptionOutOfRange );
	CPPUNIT_TEST( testCapacity );
	CPPUNIT_TEST( testTypeClass );
	CPPUNIT_TEST_SUITE_END();

protected:

public:


	class TestObj: public iObject {
	public:

		STAMINA_OBJECT_CLASS(TestObj, iObject);

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

	typedef StaticPtr<TestObj> TestPtr;

	typedef Array<TestObj> tArray;
	typedef Array<TestPtr> tArrayP;

	typedef Stamina::oArray<TestObj> oArray;
	typedef Stamina::oArray<TestPtr> oArrayP;

	void setUp() {

	}
	void tearDown() {
	}

	oArray build(int count = standardCount, int start = 0) {
		oArray arr = new tArray(count);
		while(count--) {
			arr->insert(TestObj(start++));
		}
		return arr;
	}

	static const int standardCount = 50;


protected:


	void testInsert() {
		// kolejne na koniec...
		{
			CPPUNIT_ASSERT_EQUAL(0, TestObj::instances);
			oArray a = new tArray();
			CPPUNIT_ASSERT_EQUAL((unsigned)0, a->size());
			a->insert(TestObj(0));
			a->insert(TestObj(1));
			a->insert(TestObj(2));
			CPPUNIT_ASSERT_EQUAL((unsigned)3, a->size());
			CPPUNIT_ASSERT_EQUAL(3, TestObj::instances);
			iArrayBase* aaa = a;
			CPPUNIT_ASSERT_EQUAL(0, a->at(0).a);
			CPPUNIT_ASSERT_EQUAL(1, a->at(1).a);
			CPPUNIT_ASSERT_EQUAL(2, a->at(2).a);
			CPPUNIT_ASSERT_EQUAL(2, a->at(-1).a);
			CPPUNIT_ASSERT_EQUAL(1, a->at(-2).a);
			CPPUNIT_ASSERT_EQUAL(0, a->at(-3).a);
		}
		CPPUNIT_ASSERT_EQUAL(0, TestObj::instances);
		// kolejne na pocz¹tek...
		{
			oArray a = new tArray();
			a->insert(TestObj(0), 0);
			a->insert(TestObj(1), 0);
			a->insert(TestObj(2), 0);
			CPPUNIT_ASSERT_EQUAL((unsigned)3, a->size());
			CPPUNIT_ASSERT_EQUAL(3, TestObj::instances);
			CPPUNIT_ASSERT_EQUAL(2, a->at(0).a);
			CPPUNIT_ASSERT_EQUAL(1, a->at(1).a);
			CPPUNIT_ASSERT_EQUAL(0, a->at(2).a);
		}
		CPPUNIT_ASSERT_EQUAL(0, TestObj::instances);
		// mieszanka...
		{
			oArray a = new tArray();
			a->insert(TestObj(0), 0); //0
			a->insert(TestObj(1), 5); //01
			a->insert(TestObj(2), 0); //201
			a->insert(TestObj(3), 1); //2301
			a->insert(TestObj(4), -1); //23041
			a->insert(TestObj(5), 5); //230415
			CPPUNIT_ASSERT_EQUAL((unsigned)6, a->size());
			CPPUNIT_ASSERT_EQUAL(6, TestObj::instances);
			CPPUNIT_ASSERT_EQUAL(2, a->at(0).a);
			CPPUNIT_ASSERT_EQUAL(3, a->at(1).a);
			CPPUNIT_ASSERT_EQUAL(0, a->at(2).a);
			CPPUNIT_ASSERT_EQUAL(4, a->at(3).a);
			CPPUNIT_ASSERT_EQUAL(1, a->at(4).a);
			CPPUNIT_ASSERT_EQUAL(5, a->at(5).a);
		}
		CPPUNIT_ASSERT_EQUAL(0, TestObj::instances);
	}

	void testErase() {
		{ // z koñca
			oArray a = build();
			CPPUNIT_ASSERT_EQUAL(standardCount, TestObj::instances);
			a->erase(-1, 1);
			a->erase(-2, 2);
			CPPUNIT_ASSERT_EQUAL(standardCount - 3, TestObj::instances);
			CPPUNIT_ASSERT_EQUAL((int)a->size() - 3, a->at(-3).a);
		}
		{ // z pocz¹tku
			oArray a = build();
			CPPUNIT_ASSERT_EQUAL(standardCount, TestObj::instances);
			a->erase(0, 1);
			a->erase(0, 2);
			CPPUNIT_ASSERT_EQUAL(standardCount - 3, TestObj::instances);
			CPPUNIT_ASSERT_EQUAL(3, a->at(0).a);
		}
		{ // ca³oœæ
			oArray a = build();
			CPPUNIT_ASSERT_EQUAL(standardCount, TestObj::instances);
			a->erase(2);
			a->erase(0);
			a->erase(0); // ju¿ jest pusty, ale NIE powinien wyrzuciæ throw!
			CPPUNIT_ASSERT_EQUAL(0, TestObj::instances);
			CPPUNIT_ASSERT_EQUAL(0, (int)a->size());
			a->clear();
			CPPUNIT_ASSERT_EQUAL(0, TestObj::instances);
			CPPUNIT_ASSERT_EQUAL(0, (int)a->size());
		}
	}

	void testOutOfRange1() { 
		oArray a = build();
		a->at( standardCount );
	}

	void testOutOfRange2() { //ujemne
		oArray a = build();
		a->at( -standardCount - 1 );
	}

	void testEraseOutOfRange1() { 
		oArray a = new tArray();
		a->erase( -1 );
	}

	void testEraseOutOfRange2() { 
		oArray a = build();
		a->erase( iArrayBase::wholeData );
	}

	void testCapacity() {
		oArray a = new tArray();

		CPPUNIT_ASSERT_EQUAL((unsigned)0, a->capacity());
		a->reserve(12);
		CPPUNIT_ASSERT_EQUAL((unsigned)12, a->capacity());
		a->reserve(600);
		CPPUNIT_ASSERT_EQUAL((unsigned)600, a->capacity());
		a->reserve(100);
		CPPUNIT_ASSERT_EQUAL((unsigned)600, a->capacity());

		oArrayP b = new tArrayP();

		CPPUNIT_ASSERT_EQUAL((unsigned)0, b->capacity());
		b->reserve(12);
		CPPUNIT_ASSERT_EQUAL((unsigned)16, b->capacity());

	}

	void testTypeClass() {

		typedef SharedPtr<iArrayBase> A;
		A a = new Array<TestObj>();
		A b = new Array<TestPtr>();
		A c = new Array<int>();

		CPPUNIT_ASSERT( a->getTypeClass() == TestObj::staticClassInfo() );
		CPPUNIT_ASSERT( b->getTypeClass() == TestPtr::staticClassInfo() );
		CPPUNIT_ASSERT( b->getTypeClass().inheritsFrom(TestObj::staticClassInfo()) );
		CPPUNIT_ASSERT( b->getTypeClass().getBaseInfo() == TestObj::staticClassInfo() );
		CPPUNIT_ASSERT( c->getTypeClass() == ObjectClassInfo::none );

	}

};

int TestArray::TestObj::instances = 0;


CPPUNIT_TEST_SUITE_REGISTRATION( TestArray );

