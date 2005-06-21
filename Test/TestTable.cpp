#include <stdafx.h>
#include <cppunit/extensions/HelperMacros.h>
#include "..\DataTable.h"
#include <Stamina\MD5.h>
#include <Stamina\Helpers.h>

using namespace Stamina;
using namespace Stamina::DT;


class TestTable : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestTable );
  
	CPPUNIT_TEST( testInsertRow );
	CPPUNIT_TEST( testDeleteRow );
	CPPUNIT_TEST( testClearRows );
	CPPUNIT_TEST( testLock );
	CPPUNIT_TEST( testSetPassword );
	CPPUNIT_TEST( testParams );
	CPPUNIT_TEST( testGetSetValue );
	CPPUNIT_TEST( testFindRow );

    CPPUNIT_TEST_SUITE_END();

protected:
  
	ColumnsDesc _cols;
	static const tColId colInt = 1;
	static const tColId colString = 2;
	static const tColId colIntDef = 3;
	static const tColId colStringDef = 4;
	static const tColId col64 = 5;
	static const tColId colBin = 6;

public:
	void setUp() {
		_cols.setColumn(colInt, ctypeInt, 0, "Int");
		_cols.setColumn(colIntDef, ctypeInt, (DataEntry) colIntDef, "IntDef");
		_cols.setColumn(colString, ctypeString, 0, "String");
		_cols.setColumn(colStringDef, ctypeString, "StringDef", "StringDef");
		_cols.setColumn(col64, ctype64, 0, "64");
		_cols.setColumn(colBin, ctypeBin, 0, "Bin");
	}
	void tearDown() {
	}

protected:

	void testInsertRow() {
		DataTable dt;
		dt.mergeColumns(_cols);
		CPPUNIT_ASSERT( dt.addRow() != rowNotFound );
		CPPUNIT_ASSERT( dt.addRow(10) != rowNotFound );
		CPPUNIT_ASSERT( dt.insertRow(1, DataTable::flagId(10)) == rowNotFound );
		CPPUNIT_ASSERT( dt.insertRow(1, 11) != rowNotFound );
		CPPUNIT_ASSERT( dt.getRow(1).getId() == DataTable::flagId(11) );
		CPPUNIT_ASSERT( dt.getRowPos(DataTable::flagId(11)) == 1 );
		CPPUNIT_ASSERT( dt.getRowCount() == 3 );
	}

	void testDeleteRow() {
		DataTable dt;
		dt.mergeColumns(_cols);
		dt.addRow(1);
		tRowId id = dt.addRow(2);
		tRowId lastId = dt.addRow(3);
		CPPUNIT_ASSERT( dt.deleteRow(id) );
		CPPUNIT_ASSERT( dt.getRowCount() == 2 );
		CPPUNIT_ASSERT( dt.getRowPos(lastId) == 1 );
	}

	void testClearRows() {
		DataTable dt;
		dt.mergeColumns(_cols);
		dt.addRow();
		dt.addRow();
		dt.addRow();
		dt.clearRows();
		CPPUNIT_ASSERT( dt.getRowCount() == 0 );
	}

	void testLock() {
		DataTable dt;
		dt.mergeColumns(_cols);
		dt.addRow();
		dt.addRow();
		dt.addRow();
		dt.lock(allRows);
		dt.unlock(allRows);
	}

	void testSetPassword() {
		DataTable dt;
		dt.setPassword("abc");
		CPPUNIT_ASSERT( dt.getPasswordDigest().empty() == false );
	}

	void testParams() {
		DataTable dt;
		dt.setParam("a", "1");
		dt.setParam("b", "2");
		CPPUNIT_ASSERT( dt.getParam("a") == "1" );
		dt.setParam("a", "3");
		CPPUNIT_ASSERT( dt.getParam("a") == "3" );
		CPPUNIT_ASSERT( dt.paramExists("a") == true );
		dt.resetParam("a");
		CPPUNIT_ASSERT( dt.paramExists("a") == false );
	}

	void testGetSetValue() {
		std::string testString = "100sdfksdfkjsdhfsjfhsfjkhsdf jks hfjk hfdsjkfh s";
		int testInt = 100;
		__int64 test64 = 0xFF00000000;
		TypeBin testBin;
		testBin.size = 10;
		testBin.buff = "1234567890";
		char buffer [50];


		DataTable dt;
		dt.mergeColumns(_cols);
		tRowId row = dt.addRow();

		// getting default values...
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 0 );
		CPPUNIT_ASSERT( dt.getInt(row, colIntDef) == colIntDef );
		CPPUNIT_ASSERT( dt.getStr(row, colString) == "" );
		CPPUNIT_ASSERT( dt.getStr(row, colStringDef) == "StringDef" );

		// setting values...

		CPPUNIT_ASSERT( dt.setStr(row, colString, testString.c_str()) );
		CPPUNIT_ASSERT( dt.setInt(row, colInt, testInt) );
		CPPUNIT_ASSERT( dt.set64(row, col64, test64) );
		CPPUNIT_ASSERT( dt.setBin(row, colBin, testBin) );

		// getting values
		CPPUNIT_ASSERT_EQUAL( testInt, dt.getInt(row, colInt) );
		CPPUNIT_ASSERT_EQUAL( test64, dt.get64(row, col64) );
		// duplicate -- String
		CPPUNIT_ASSERT_EQUAL( testString, dt.getStr(row, colString) );
		// direct
		CPPUNIT_ASSERT( dt.getCh(row, colString, (char*) -1) == testString );
		// size
		{
			Value v = ValueStrGetSize();
			CPPUNIT_ASSERT( dt.getValue(row, colString, v) );
			CPPUNIT_ASSERT( v.buffSize > 0 );
		}
		// copy
		CPPUNIT_ASSERT_EQUAL( testString.substr(0, 9), std::string(dt.getCh(row, colString, buffer, 10)) );
		// duplicate -- Bin
		{
			TypeBin bin(0, -1);
			CPPUNIT_ASSERT( dt.getBin (row, colBin, bin) == testBin );
			free( bin.buff );
		}
		// direct
		CPPUNIT_ASSERT( dt.getBin (row, colBin, TypeBin((void*)-1, 0)) == testBin );
		// size
		CPPUNIT_ASSERT( dt.getBin (row, colBin, TypeBin(0, 0)).size > 0 );
		// copy
		CPPUNIT_ASSERT( dt.getBin (row, colBin, TypeBin(buffer, 10)) == testBin );


		// getting conversions
		CPPUNIT_ASSERT( dt.getInt(row, colString) == testInt );
		CPPUNIT_ASSERT( dt.getInt(row, col64) == 0 );
		CPPUNIT_ASSERT( dt.getInt(row, colBin) == 0 );

		CPPUNIT_ASSERT( dt.get64(row, colString) == testInt );
		CPPUNIT_ASSERT( dt.get64(row, colInt) == testInt );
		
		CPPUNIT_ASSERT( dt.getStr(row, colInt) == inttostr(testInt) );
		CPPUNIT_ASSERT( dt.getStr(row, col64) == _i64toa(test64, buffer, 10) );
		// reuse
		CPPUNIT_ASSERT( dt.getCh(row, colInt, buffer) == inttostr(testInt) );
		// copy
		CPPUNIT_ASSERT( dt.getCh(row, colInt, buffer, 10) == inttostr(testInt) );

		// setting conversions
		CPPUNIT_ASSERT( dt.setInt(row, colString, testInt) );
		CPPUNIT_ASSERT( dt.getStr(row, colString) == inttostr(testInt) );

		CPPUNIT_ASSERT( dt.setInt(row, col64, testInt) );
		CPPUNIT_ASSERT( dt.get64(row, col64) == testInt );

		CPPUNIT_ASSERT( dt.setInt(row, colBin, testInt) == false );

		CPPUNIT_ASSERT( dt.set64(row, colString, test64) );
		CPPUNIT_ASSERT( dt.getStr(row, colString) == _i64toa(test64, buffer, 10) );
		CPPUNIT_ASSERT( dt.set64(row, colInt, test64) );
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 0 );

		CPPUNIT_ASSERT( dt.setStr(row, colInt, "1200") );
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 1200 );

		CPPUNIT_ASSERT( dt.setStr(row, col64, "0xABC12345678") );
		CPPUNIT_ASSERT_EQUAL( 0xABC12345678, dt.get64(row, col64) );

}

	void testFindRow() {
		DataTable dt;
		dt.mergeColumns(_cols);

		tRowId r1 = dt.addRow();
		tRowId r2 = dt.addRow();
		tRowId r3 = dt.addRow();

		dt.setInt(r1, colInt, 100);
		dt.setInt(r2, colInt, 200);
		dt.setInt(r3, colInt, 300);

		dt.setStr(r1, colString, "a");
		dt.setStr(r2, colString, "b");
		dt.setStr(r3, colString, "c");

		dt.setStr(r2, colStringDef, "Inny");

		CPPUNIT_ASSERT( dt.findRow(0, Find(Find::neq, colStringDef, ValueStr("StringDef"))) == r2 );

		CPPUNIT_ASSERT( dt.findRow(0, Find::EqStr(colString, "b")) == r2 );
		CPPUNIT_ASSERT( dt.findRow(r2, Find(Find::neq, colString, ValueStr("b"))) == r3);

		CPPUNIT_ASSERT( dt.findRow(0, Find(Find::neq, colString, ValueStr("a")), Find(Find::gteq, colInt, ValueInt(100))) == r2);

		CPPUNIT_ASSERT( dt.findRow(0, Find::EqStr(colString, "a"), Find(Find::gt, colInt, ValueInt(100))) == rowNotFound );

	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestTable );


