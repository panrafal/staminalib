#include <stdafx.h>
#include <cppunit/extensions/HelperMacros.h>
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
	CPPUNIT_TEST( testExpandCount );
	CPPUNIT_TEST( testSetColumns );
	CPPUNIT_TEST( testJoin );
	CPPUNIT_TEST( testJoinOverwrite );
	CPPUNIT_TEST( testOptimize );

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
		CPPUNIT_ASSERT( cols.getColumnByIndex(9).empty() == true );
	}
	void testExpandCount() {
		ColumnsDesc cols;
		cols.setColumnCount(10, false);
		cols.setColumnCount(10, true);
		CPPUNIT_ASSERT( cols.getColCount() == 20);
		cols.setColumn((tColId) 19, ctypeInt);
		CPPUNIT_ASSERT( cols.getColumnByIndex(19).empty() == true );
		CPPUNIT_ASSERT( cols.getColumn((tColId) 19).getType() == ctypeInt );
	}
	void testSetColumns() {
		ColumnsDesc cols;
		cols.setColumnCount(1, false);
		CPPUNIT_ASSERT( cols.setColumn((tColId)1, ctypeString | cflagXor, "Hmmm?") != colNotFound );
		CPPUNIT_ASSERT( cols.setColumn((tColId)2, ctypeString | cflagXor, "Hmmm?", "2") != colNotFound );
		CPPUNIT_ASSERT( cols.setColumn(colByName, ctypeString | cflagXor, "Hmmm?", "3") != colNotFound );

		CPPUNIT_ASSERT( cols.getColumnByIndex(0).getId() == 1 );
		CPPUNIT_ASSERT( cols.getColumn((tColId)2).getType() == ctypeString);
		CPPUNIT_ASSERT( cols.getColumn("3").getId() != colByName );
		CPPUNIT_ASSERT( cols.getColumn("3").getFlags() == (ctypeString | cflagXor) );
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
		CPPUNIT_ASSERT( cols1.getColumn("1").getType() == ctypeInt );
		CPPUNIT_ASSERT( cols1.getColumn((tColId)3).getType() == ctypeString );
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
		CPPUNIT_ASSERT( cols1.getColumn("1").getType() == ctypeString );
		CPPUNIT_ASSERT( cols1.getColumn((tColId)3).getType() == ctypeString );
	}

	void testOptimize() {
		ColumnsDesc cols;
		cols.setColumnCount(10);
		cols.setUniqueCol("1", ctypeInt);
		cols.optimize();
		CPPUNIT_ASSERT( cols.getColCount() == 1);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestColumns );


