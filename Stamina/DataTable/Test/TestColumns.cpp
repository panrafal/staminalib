#include <stdafx.h>
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina/VersionControl.h>
#include "..\Column.h"
#include "..\DataTable.h"
#include <Stamina\MD5.h>
#include <Stamina\Helpers.h>

using namespace Stamina;
using namespace Stamina::DT;

const int timingLoops = 2000;

class TestColumns : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestColumns );
  
	CPPUNIT_TEST( testSetCount );
	CPPUNIT_TEST( testSetColumns );
	CPPUNIT_TEST( testExpandCount );
	CPPUNIT_TEST( testDefaults );
	CPPUNIT_TEST( testClone );
	CPPUNIT_TEST( testJoin );
	CPPUNIT_TEST( testJoinOverwrite );


	CPPUNIT_TEST_SUITE_END();

protected:

public:
	void setUp() {
	}
	void tearDown() {
	}

protected:

	void testSetCount() {
		ColumnsDesc cols;
		cols.setColumnCount(10);
		CPPUNIT_ASSERT( cols.getColumnByIndex(9)->isUndefined() == true );
	}
	void testExpandCount() {
		ColumnsDesc cols;
		cols.setColumnCount(10, false);
		cols.setColumnCount(10, true);
		CPPUNIT_ASSERT( cols.getColCount() == 20);
		cols.setColumn((tColId) 19, ctypeInt);
		CPPUNIT_ASSERT( cols.getColumnByIndex(19)->isUndefined() == true );
		CPPUNIT_ASSERT( cols.getColumn((tColId) 19)->getType() == ctypeInt );
	}
	void testSetColumns() {
		ColumnsDesc cols;
		cols.setColumnCount(1, false);

		CPPUNIT_ASSERT( cols.setColumn((tColId)1, ctypeString | cflagXor) == true );
		CPPUNIT_ASSERT( cols.setColumn((tColId)2, ctypeString | cflagXor, "2") == true );
		CPPUNIT_ASSERT( cols.setColumn(colByName, ctypeString | cflagXor, "3") == true );
		CPPUNIT_ASSERT( cols.setColumn(colByName, ctypeString | cflagXor, "4") == true );

		CPPUNIT_ASSERT( cols.getColumnByIndex(0)->getId() == 1 );
		CPPUNIT_ASSERT( cols.getColumn((tColId)2)->getType() == ctypeString);
		CPPUNIT_ASSERT( cols.getColumn("3")->getId() != colByName );
		CPPUNIT_ASSERT( cols.getColumn("3")->getFlags() == (ctypeString | cflagXor | cflagIsDefined) );
	}

	void testDefaults() {
		ColumnsDesc cols;
		oColumn col;

		col = cols.setColumn(colByName, ctypeInt, "1");
		CPPUNIT_ASSERT_EQUAL( (int)0, col->getInt(rowDefault) );
		col->setInt(rowDefault, 10);
		CPPUNIT_ASSERT_EQUAL( (int)10, col->getInt(rowDefault) );
		CPPUNIT_ASSERT_EQUAL( String("10"), col->getString(rowDefault) );

		col = cols.setColumn(colByName, ctypeInt64, "2");
		CPPUNIT_ASSERT_EQUAL( (__int64)0, col->getInt64(rowDefault) );
		col->setInt64(rowDefault, 10);
		CPPUNIT_ASSERT_EQUAL( (__int64)10, col->getInt64(rowDefault) );
		CPPUNIT_ASSERT_EQUAL( (int)10, col->getInt(rowDefault) );

		col = cols.setColumn(colByName, ctypeDouble, "3");
		CPPUNIT_ASSERT_EQUAL( (double)0, col->getDouble(rowDefault) );
		col->setDouble(rowDefault, 10);
		CPPUNIT_ASSERT_EQUAL( (double)10, col->getDouble(rowDefault) );

		col = cols.setColumn(colByName, ctypeString, "4");
		CPPUNIT_ASSERT_EQUAL( String(""), col->getString(rowDefault) );
		col->setString(rowDefault, "Hello!");
		CPPUNIT_ASSERT_EQUAL( String("Hello!"), col->getString(rowDefault) );

		col = cols.setColumn(colByName, ctypeBin, "5");
		ByteBuffer bin;
		bin.assignCheapReference((unsigned char*)"Hello!", 6);
		CPPUNIT_ASSERT( col->getBin(rowDefault).getLength() == 0 );
		col->setBin(rowDefault, bin);
		CPPUNIT_ASSERT( memcmp(bin.getString(), col->getBin(rowDefault).getString(), 6) == 0 );
		CPPUNIT_ASSERT_EQUAL( (int)0, col->getInt(rowDefault) );
	}

	void testClone() {
		ColumnsDesc cols;
		oColumn col1, col2, col3, col4;

		col1 = cols.setColumn(colByName, ctypeInt, "1");
		col1->setInt(rowDefault, 10);

		col2 = cols.setColumn(colByName, ctypeString, "2");
		col2->setString(rowDefault, "Hello!");

		col3 = col1->cloneColumn();
		col4 = col2->cloneColumn();

		CPPUNIT_ASSERT_EQUAL( col1->getType(), col3->getType() );
		CPPUNIT_ASSERT_EQUAL( col2->getType(), col4->getType() );

		CPPUNIT_ASSERT_EQUAL( col1->getId(), col3->getId() );
		CPPUNIT_ASSERT_EQUAL( col2->getId(), col4->getId() );

		CPPUNIT_ASSERT_EQUAL( col1->getInt(rowDefault), col3->getInt(rowDefault) );
		CPPUNIT_ASSERT_EQUAL( col2->getString(rowDefault), col4->getString(rowDefault) );
		CPPUNIT_ASSERT( col2->getString(rowDefault).a_str() != col4->getString(rowDefault).a_str() );


	}

	void testJoin() {
		ColumnsDesc cols1;
		ColumnsDesc cols2;
		cols1.setUniqueCol("1", ctypeInt);
		cols1.setColumn((tColId)2, ctypeInt);

		cols2.setUniqueCol("1", ctypeString);
		cols2.setColumn((tColId)3, ctypeString);

		cols1.join(cols2, false);

		CPPUNIT_ASSERT( cols1.getColCount() == 3);
		CPPUNIT_ASSERT( cols1.getColumn("1")->getType() == ctypeInt );
		CPPUNIT_ASSERT( cols1.getColumn((tColId)3)->getType() == ctypeString );
	}
	void testJoinOverwrite() {
		ColumnsDesc cols1;
		ColumnsDesc cols2;
		cols1.setColumnCount(2);
		cols2.setColumnCount(2);
		cols1.setUniqueCol("1", ctypeInt);
		cols1.setColumn((tColId)2, ctypeInt);

		cols2.setUniqueCol("1", ctypeString);
		cols2.setColumn((tColId)3, ctypeString);

		cols1.join(cols2, true);

		CPPUNIT_ASSERT( cols1.getColCount() == 3);
		CPPUNIT_ASSERT( cols1.getColumn("1")->getType() == ctypeString );
		CPPUNIT_ASSERT( cols1.getColumn((tColId)3)->getType() == ctypeString );
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestColumns );


