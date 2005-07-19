#include <stdafx.h>
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina/VersionControl.h>
#include "..\DataTable.h"
#include <Stamina\MD5.h>
#include <Stamina\Helpers.h>
#include <Stamina\WideChar.h>

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
	CPPUNIT_TEST( testGetSetEmptyValue );
	CPPUNIT_TEST( testFindRow );
	CPPUNIT_TEST( testDropDefault );

    CPPUNIT_TEST_SUITE_END();

protected:
  
	ColumnsDesc _cols;
	static const tColId colInt = 1;
	static const tColId colString = 2;
	static const tColId colIntDef = 3;
	static const tColId colStringDef = 4;
	static const tColId col64 = 5;
	static const tColId col64Def = 6;
	static const tColId colBin = 7;
	static const tColId colBinDef = 8;
	static const tColId colWide = 9;
	static const tColId colWideDef = 10;

	std::string testString, testStringDef;
	std::wstring testWide, testWideDef;
	int testInt, testIntDef;
	__int64 test64, test64Def;
	TypeBin testBin, testBinDef;

public:
	void setUp() {
		testString = "100sdfksdfkjsdhfsjfhsfjkhsdf jks hfjk hfdsjkfh s";
		testWide = L"84765F KJDFH JKFHDSFH SDFHSHFSDHF SDFH DSJFHDF H";
		testInt = 100;
		testIntDef = 1000;
		test64 = 0xFF00000000;
		test64Def = 0xAA00000001;
		testBin.size = 10;
		testBin.buff = "1234567890";
		testBinDef.size = 5;
		testBinDef.buff = "12345";
		testStringDef = "DEFAULT";
		testWideDef = L"default";

		_cols.setColumn(colInt, ctypeInt , 0, "Int");
		_cols.setColumn(colIntDef, ctypeInt | cflagXor, (DataEntry) testIntDef, "IntDef");
		_cols.setColumn(colString, ctypeString, 0, "String");
		_cols.setColumn(colStringDef, ctypeString | cflagXor, (void*)testStringDef.c_str(), "StringDef");
		_cols.setColumn(col64, ctype64, 0, "64");
		_cols.setColumn(col64Def, ctype64 | cflagXor, &test64Def, "64Def");
		_cols.setColumn(colBin, ctypeBin, 0, "Bin");
		_cols.setColumn(colBinDef, ctypeBin | cflagXor, &testBinDef, "BinDef");
		_cols.setColumn(colWide, ctypeWideString, 0, "Wide");
		_cols.setColumn(colWideDef, ctypeWideString | cflagXor, (void*)testWideDef.c_str(), "WideDef");
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
		testDoGetSetValue(true);
	}
	void testGetSetEmptyValue() {
		testDoGetSetValue(false);
	}

	void testDoGetSetValue(bool values) {
		std::string testString = values ? "100sdfksdfkjsdhfsjfhsfjkhsdf jks hfjk hfdsjkfh s" : "";
		std::wstring testWide = values ? L"100mmm485487FJGFJGDFG SDFGSD FSGDFJ HGSDHF GSDHF " : L"";
		int testInt = values ? 100 : 0;
		__int64 test64 = values ? 0xFF00000000 : 0;
		TypeBin testBin;
		if (values) {
			testBin.size = 10;
			testBin.buff = "1234567890";
		}
		char buffer [50];


		DataTable dt;
		dt.mergeColumns(_cols);
		tRowId row = dt.addRow();

		// getting default values...
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 0 );
		CPPUNIT_ASSERT( dt.getInt(row, colIntDef) == testIntDef );
		CPPUNIT_ASSERT( dt.getStr(row, colString) == "" );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt.getStr(row, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( std::wstring(L""), dt.getWStr(row, colWide) );
		CPPUNIT_ASSERT_EQUAL( testWideDef, dt.getWStr(row, colWideDef) );
		CPPUNIT_ASSERT( dt.get64(row, col64) == 0 );
		CPPUNIT_ASSERT( dt.get64(row, col64Def) == test64Def );
		CPPUNIT_ASSERT( dt.getBin(row, colBin, TypeBin(buffer, 10)) == TypeBin(0, 0) );
		CPPUNIT_ASSERT( dt.getBin(row, colBinDef, TypeBin(buffer, 10)) == testBinDef );

		// setting values...

		if (values) {
			CPPUNIT_ASSERT( dt.setStr(row, colString, testString.c_str()) );
			CPPUNIT_ASSERT( dt.setWStr(row, colWide, testWide.c_str()) );
			CPPUNIT_ASSERT( dt.setInt(row, colInt, testInt) );
			CPPUNIT_ASSERT( dt.set64(row, col64, test64) );
			CPPUNIT_ASSERT( dt.setBin(row, colBin, testBin) );
		}

		// getting values
		CPPUNIT_ASSERT_EQUAL( testInt, dt.getInt(row, colInt) );
		CPPUNIT_ASSERT_EQUAL( test64, dt.get64(row, col64) );
		// duplicate -- String
		CPPUNIT_ASSERT_EQUAL( testString, dt.getStr(row, colString) );
		CPPUNIT_ASSERT_EQUAL( testWide, dt.getWStr(row, colWide) );
		// direct
		CPPUNIT_ASSERT( dt.getCh(row, colString, (char*) -1) == testString );
		CPPUNIT_ASSERT( dt.getWCh(row, colWide, (wchar_t*) -1) == testWide );
		// size
		{
			Value v = ValueStrGetSize();
			CPPUNIT_ASSERT( dt.getValue(row, colString, v) );
			CPPUNIT_ASSERT( values ? v.buffSize > 0 : v.buffSize == 1 );
		}
		{
			Value v = ValueWideStrGetSize();
			CPPUNIT_ASSERT( dt.getValue(row, colWide, v) );
			CPPUNIT_ASSERT( values ? v.buffSize > 0 : v.buffSize == 2 );
		}
		// copy
		CPPUNIT_ASSERT_EQUAL( testString.substr(0, 9), std::string(dt.getCh(row, colString, buffer, 10)) );

		CPPUNIT_ASSERT_EQUAL( testWide.substr(0, 9), std::wstring(dt.getWCh(row, colWide, (wchar_t*)buffer, 20)) );

		// duplicate -- Bin
		{
			TypeBin bin(0, -1);
			CPPUNIT_ASSERT( dt.getBin (row, colBin, bin) == testBin );
			free( bin.buff );
		}
		// direct
		CPPUNIT_ASSERT( dt.getBin (row, colBin, TypeBin((void*)-1, 0)) == testBin );
		// size
		if (values) {
			CPPUNIT_ASSERT( dt.getBin (row, colBin, TypeBin(0, 0)).size > 0);
		} else {
			CPPUNIT_ASSERT( dt.getBin (row, colBin, TypeBin(0, 0)).size == 0);
		}
		// copy
		CPPUNIT_ASSERT( dt.getBin (row, colBin, TypeBin(buffer, 10)) == testBin );


		// getting conversions
		CPPUNIT_ASSERT( dt.getInt(row, colString) == testInt );
		CPPUNIT_ASSERT( dt.getInt(row, colWide) == testInt );
		CPPUNIT_ASSERT( dt.getInt(row, col64) == 0 );
		CPPUNIT_ASSERT( dt.getInt(row, colBin) == 0 );

		CPPUNIT_ASSERT( dt.get64(row, colString) == testInt );
		CPPUNIT_ASSERT( dt.get64(row, colWide) == testInt );
		CPPUNIT_ASSERT( dt.get64(row, colInt) == testInt );
		
		CPPUNIT_ASSERT( dt.getStr(row, colWide) == testWide );
		CPPUNIT_ASSERT( dt.getStr(row, colInt) == inttostr(testInt) );
		CPPUNIT_ASSERT( dt.getStr(row, col64) == _i64toa(test64, buffer, 10) );

		CPPUNIT_ASSERT( dt.getWStr(row, colString) == testString );
		CPPUNIT_ASSERT( dt.getWStr(row, colInt) == inttostr(testInt) );
		CPPUNIT_ASSERT( dt.getWStr(row, col64) == _i64toa(test64, buffer, 10) );

		// reuse
		CPPUNIT_ASSERT( dt.getCh(row, colInt, buffer) == inttostr(testInt) );
		CPPUNIT_ASSERT( dt.getWCh(row, colInt, (wchar_t*)buffer) == inttostr(testInt) );
		// copy
		CPPUNIT_ASSERT( dt.getCh(row, colInt, buffer, 10) == inttostr(testInt) );
		CPPUNIT_ASSERT( dt.getWCh(row, colInt, (wchar_t*)buffer, 10) == inttostr(testInt) );

		// setting conversions
		CPPUNIT_ASSERT( dt.setInt(row, colString, testInt) );
		CPPUNIT_ASSERT( dt.getStr(row, colString) == inttostr(testInt) );

		CPPUNIT_ASSERT( dt.setInt(row, colWide, testInt) );
		CPPUNIT_ASSERT( dt.getWStr(row, colWide) == inttostr(testInt) );

		CPPUNIT_ASSERT( dt.setInt(row, col64, testInt) );
		CPPUNIT_ASSERT( dt.get64(row, col64) == testInt );

		CPPUNIT_ASSERT( dt.setInt(row, colBin, testInt) == false );

		CPPUNIT_ASSERT( dt.set64(row, colString, test64) );
		CPPUNIT_ASSERT( dt.getStr(row, colString) == _i64toa(test64, buffer, 10) );
		CPPUNIT_ASSERT( dt.set64(row, colInt, test64) );
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 0 );
		CPPUNIT_ASSERT( dt.set64(row, colWide, test64) );
		CPPUNIT_ASSERT( dt.getWStr(row, colWide) == _i64tow(test64, (wchar_t*)buffer, 10) );

		CPPUNIT_ASSERT( dt.setStr(row, colInt, "1200") );
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 1200 );

		CPPUNIT_ASSERT( dt.setWStr(row, colInt, L"1200") );
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 1200 );


		CPPUNIT_ASSERT( dt.setStr(row, col64, "0xABC12345678") );
		CPPUNIT_ASSERT_EQUAL( 0xABC12345678, dt.get64(row, col64) );

		CPPUNIT_ASSERT( dt.setWStr(row, col64, L"0xABC12345678") );
		CPPUNIT_ASSERT_EQUAL( 0xABC12345678, dt.get64(row, col64) );

		CPPUNIT_ASSERT( dt.setStr(row, colWide, testString) );
		CPPUNIT_ASSERT_EQUAL( testString, dt.getStr(row, colWide) );

		CPPUNIT_ASSERT( dt.setWStr(row, colString, testWide) );
		CPPUNIT_ASSERT_EQUAL( testWide, dt.getWStr(row, colString) );
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
		dt.setWStr(r3, colWide, L"d");

		dt.setStr(r2, colStringDef, "Inny");

		CPPUNIT_ASSERT( dt.findRow(0, Find(Find::neq, colStringDef, ValueStr(testStringDef.c_str()))) == r2 );

		CPPUNIT_ASSERT( dt.findRow(0, Find::EqStr(colString, "b")) == r2 );
		CPPUNIT_ASSERT( dt.findRow(r2, Find(Find::neq, colString, ValueStr("b"))) == r3);

		CPPUNIT_ASSERT( dt.findRow(0, Find(Find::neq, colString, ValueStr("a")), Find(Find::gteq, colInt, ValueInt(100))) == r2);

		CPPUNIT_ASSERT( dt.findRow(0, Find::EqStr(colString, "a"), Find(Find::gt, colInt, ValueInt(100))) == rowNotFound );

		CPPUNIT_ASSERT( dt.findRow(0, Find::EqWStr(colWide, L"d")) == r3 );
	}


	void testDropDefault() {
		char buffer [50];

		DataTable dt;
		dt.mergeColumns(_cols);
		tRowId row = dt.addRow();

		// setting values
		CPPUNIT_ASSERT( dt.setStr(row, colStringDef, testString.c_str()) );
		CPPUNIT_ASSERT( dt.setWStr(row, colWideDef, testWide.c_str()) );
		CPPUNIT_ASSERT( dt.setInt(row, colIntDef, testInt) );
		CPPUNIT_ASSERT( dt.set64(row, col64Def, test64) );
		CPPUNIT_ASSERT( dt.setBin(row, colBinDef, testBin) );

		// setting defaults
		CPPUNIT_ASSERT( dt.setStr(row, colStringDef, testStringDef.c_str(), true) );
		CPPUNIT_ASSERT( dt.setWStr(row, colWideDef, testWideDef.c_str(), true) );
		CPPUNIT_ASSERT( dt.setInt(row, colIntDef, testIntDef, true) );
		CPPUNIT_ASSERT( dt.set64(row, col64Def, test64Def, true) );
		CPPUNIT_ASSERT( dt.setBin(row, colBinDef, testBinDef, true) );

		// getting default values...
		CPPUNIT_ASSERT( dt.getInt(row, colIntDef) == testIntDef );
		CPPUNIT_ASSERT( dt.getStr(row, colStringDef) == testStringDef );
		CPPUNIT_ASSERT( dt.getWStr(row, colWideDef) == testWideDef );
		CPPUNIT_ASSERT( dt.get64(row, col64Def) == test64Def );
		CPPUNIT_ASSERT( dt.getBin(row, colBinDef, TypeBin(buffer, 10)) == testBinDef );

		// setting values
		CPPUNIT_ASSERT( dt.setStr(row, colStringDef, testString.c_str()) );
		CPPUNIT_ASSERT( dt.setWStr(row, colWideDef, testWide.c_str()) );
		CPPUNIT_ASSERT( dt.setInt(row, colIntDef, testInt) );
		CPPUNIT_ASSERT( dt.set64(row, col64Def, test64) );
		CPPUNIT_ASSERT( dt.setBin(row, colBinDef, testBin) );

		// checking values
		CPPUNIT_ASSERT( dt.getInt(row, colIntDef) == testInt );
		CPPUNIT_ASSERT( dt.getStr(row, colStringDef) == testString );
		CPPUNIT_ASSERT( dt.getWStr(row, colWideDef) == testWide );
		CPPUNIT_ASSERT( dt.get64(row, col64Def) == test64 );
		CPPUNIT_ASSERT( dt.getBin(row, colBinDef, TypeBin(buffer, 10)) == testBin );

	}


};





CPPUNIT_TEST_SUITE_REGISTRATION( TestTable );


