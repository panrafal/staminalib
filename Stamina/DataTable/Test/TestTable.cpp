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

$Id: $

*/
#include <stdafx.h>
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina/VersionControl.h>
#include "..\Column.h"
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
	CPPUNIT_TEST( testWideString );
	CPPUNIT_TEST( testHandlers );
	CPPUNIT_TEST( testTriggers );

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

	String testString, testStringDef;
	String testWide, testWideDef;
	int testInt, testIntDef;
	__int64 test64, test64Def;
	ByteBuffer testBin, testBinDef;

public:
	void setUp() {
		testString = "100sdfksdfkjsdhfsjfhsfjkhsdf jks hfjk hfdsjkfh s";
		testWide = L"84765F KJDFH JKFHDSFH SDFHSHFSDHF SDFH DSJFHDF H";
		testInt = 100;
		testIntDef = 1000;
		test64 = 0xFF00000000;
		test64Def = 0xAA00000001;
		testBin.assign((unsigned char*)"1234567890", 10);
		testBinDef.assign((unsigned char*)"12345", 5);
		testStringDef = "DEFAULT";
		testWideDef = L"default";

		_cols.setColumn(colInt, ctypeInt , "Int");
		_cols.setColumn(colIntDef, ctypeInt | cflagXor, "IntDef")->setInt(rowDefault, testIntDef);
		_cols.setColumn(colString, ctypeString, "String");
		_cols.setColumn(colStringDef, ctypeString | cflagXor, "StringDef")->setString(rowDefault, testStringDef);
		_cols.setColumn(col64, ctype64, "64");
		_cols.setColumn(col64Def, ctype64 | cflagXor, "64Def")->setInt64(rowDefault, test64Def);
		_cols.setColumn(colBin, ctypeBin, "Bin");
		_cols.setColumn(colBinDef, ctypeBin | cflagXor, "BinDef")->setBin(rowDefault, testBinDef);
		_cols.setColumn(colWide, ctypeString, "Wide");
		_cols.setColumn(colWideDef, ctypeString | cflagXor, "WideDef")->setString(rowDefault, testWideDef);
	}
	void tearDown() {
	}

protected:

	void testInsertRow() {
		DataTable dt;
		dt.mergeColumns(_cols);
		CPPUNIT_ASSERT( dt.addRow() == true );
		CPPUNIT_ASSERT( dt.addRow(10) == true );
		CPPUNIT_ASSERT( dt.insertRow(1, DataTable::flagId(10)) == false );
		CPPUNIT_ASSERT( dt.insertRow(1, 11) == true );

		CPPUNIT_ASSERT( dt.getRow(1)->getId() == DataTable::flagId(11) );
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
		dt.lockRow(allRows);
		dt.unlockRow(allRows);
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
		String testString = values ? "100sdfksdfkjsdhfsjfhsfjkhsdf jks hfjk hfdsjkfh s" : "";
		String testWide = values ? L"100mmm485487FJGFJGDFG SDFGSD FSGDFJ HGSDHF GSDHF " : L"";
		int testInt = values ? 100 : 0;
		__int64 test64 = values ? 0xFF00000000 : 0;
		ByteBuffer testBin;
		if (values) {
			testBin.assign((unsigned char*)"1234567890", 10);
		}
		char buffer [50];


		DataTable dt;
		dt.mergeColumns(_cols);
		oRow row = dt.addRow();

		// getting default values...
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 0 );
		CPPUNIT_ASSERT( dt.getInt(row, colIntDef) == testIntDef );
		CPPUNIT_ASSERT( dt.getString(row, colString) == "" );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt.getString(row, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( String(L""), dt.getString(row, colWide) );
		CPPUNIT_ASSERT_EQUAL( testWideDef, dt.getString(row, colWideDef) );
		CPPUNIT_ASSERT( dt.get64(row, col64) == 0 );
		CPPUNIT_ASSERT( dt.get64(row, col64Def) == test64Def );
		CPPUNIT_ASSERT( dt.getBin(row, colBin) == ByteBuffer() );
		CPPUNIT_ASSERT( dt.getBin(row, colBinDef) == testBinDef );

		// setting values...

		if (values) {
			CPPUNIT_ASSERT( dt.setString(row, colString, testString) );
			CPPUNIT_ASSERT( dt.setString(row, colWide, testWide) );
			CPPUNIT_ASSERT( dt.setInt(row, colInt, testInt) );
			CPPUNIT_ASSERT( dt.set64(row, col64, test64) );
			CPPUNIT_ASSERT( dt.setBin(row, colBin, testBin) );
		}

		// getting values
		CPPUNIT_ASSERT_EQUAL( testInt, dt.getInt(row, colInt) );
		CPPUNIT_ASSERT_EQUAL( test64, dt.get64(row, col64) );
		// duplicate -- String
		CPPUNIT_ASSERT_EQUAL( testString, dt.getString(row, colString) );
		CPPUNIT_ASSERT_EQUAL( testWide, dt.getString(row, colWide) );
		// direct -- String
		CPPUNIT_ASSERT_EQUAL( testString, dt.getString(row, colString, getReference) );
		CPPUNIT_ASSERT_EQUAL( testWide, dt.getString(row, colWide, getReference) );

		// duplicate -- Bin
		CPPUNIT_ASSERT( dt.getBin (row, colBin) == testBin );
		// direct
		CPPUNIT_ASSERT( dt.getBin (row, colBin, getReference) == testBin );



		// getting conversions
		CPPUNIT_ASSERT( dt.getInt(row, colString) == testInt );
		CPPUNIT_ASSERT( dt.getInt(row, colWide) == testInt );
		CPPUNIT_ASSERT( dt.getInt(row, col64) == 0 );
		CPPUNIT_ASSERT( dt.getInt(row, colBin) == 0 );

		CPPUNIT_ASSERT( dt.get64(row, colString) == testInt );
		CPPUNIT_ASSERT( dt.get64(row, colWide) == testInt );
		CPPUNIT_ASSERT( dt.get64(row, colInt) == testInt );
		
		CPPUNIT_ASSERT( dt.getString(row, colWide) == testWide );
		CPPUNIT_ASSERT( dt.getString(row, colInt) == inttostr(testInt) );
		CPPUNIT_ASSERT( dt.getString(row, col64) == _i64toa(test64, buffer, 10) );

		// setting conversions
		CPPUNIT_ASSERT( dt.setInt(row, colString, testInt) );
		CPPUNIT_ASSERT( dt.getString(row, colString) == inttostr(testInt) );

		CPPUNIT_ASSERT( dt.setInt(row, colWide, testInt) );
		CPPUNIT_ASSERT( dt.getString(row, colWide) == inttostr(testInt) );

		CPPUNIT_ASSERT( dt.setInt(row, col64, testInt) );
		CPPUNIT_ASSERT( dt.get64(row, col64) == testInt );

		CPPUNIT_ASSERT( dt.setInt(row, colBin, testInt) == false );

		CPPUNIT_ASSERT( dt.set64(row, colString, test64) );
		CPPUNIT_ASSERT( dt.getString(row, colString) == _i64toa(test64, buffer, 10) );
		CPPUNIT_ASSERT( dt.set64(row, colInt, test64) );
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 0 );
		CPPUNIT_ASSERT( dt.set64(row, colWide, test64) );
		CPPUNIT_ASSERT( dt.getString(row, colWide) == _i64tow(test64, (wchar_t*)buffer, 10) );

		CPPUNIT_ASSERT( dt.setString(row, colInt, "1200") );
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 1200 );

		CPPUNIT_ASSERT( dt.setString(row, colInt, L"1200") );
		CPPUNIT_ASSERT( dt.getInt(row, colInt) == 1200 );


		CPPUNIT_ASSERT( dt.setString(row, col64, "0xABC12345678") );
		CPPUNIT_ASSERT_EQUAL( 0xABC12345678, dt.get64(row, col64) );

		CPPUNIT_ASSERT( dt.setString(row, col64, L"0xABC12345678") );
		CPPUNIT_ASSERT_EQUAL( 0xABC12345678, dt.get64(row, col64) );

		CPPUNIT_ASSERT( dt.setString(row, colWide, testString) );
		CPPUNIT_ASSERT_EQUAL( testString, dt.getString(row, colWide) );

		CPPUNIT_ASSERT( dt.setString(row, colString, testWide) );
		CPPUNIT_ASSERT_EQUAL( testWide, dt.getString(row, colString) );
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

		dt.setString(r1, colString, "a");
		dt.setString(r2, colString, "b");
		dt.setString(r3, colString, "c");
		dt.setString(r3, colWide, L"d");

		dt.setString(r2, colStringDef, "Inny");

		oDataRow found;

		found = dt.findRow(0, Find(Find::neq, dt.getColumn(colStringDef), new Value_string(testStringDef)));
		CPPUNIT_ASSERT( found.isValid() && found->getId() == r2 );
	
		found = dt.findRow(0, Find::EqStr(dt.getColumn(colString), "b"));
		CPPUNIT_ASSERT( found.isValid() && found->getId() == r2 );

		found = dt.findRow(r2, Find(Find::neq, dt.getColumn(colString), new Value_string("b")));
		CPPUNIT_ASSERT( found.isValid() && found->getId() == r3 );

		found = dt.findRow(0, Find(Find::neq, dt.getColumn(colString), new Value_string("a")), Find(Find::gteq, dt.getColumn(colInt), new Value_int(100)));
		CPPUNIT_ASSERT( found.isValid() && found->getId() == r2 );

		found = dt.findRow(0, Find::EqStr(dt.getColumn(colString), "a"), Find(Find::gt, dt.getColumn(colInt), new Value_int(100)));
		CPPUNIT_ASSERT( found.isValid() == false );

		found = dt.findRow(0, Find::EqStr(dt.getColumn(colWide), L"d"));
		CPPUNIT_ASSERT( found.isValid() && found->getId() == r3 );
	}


	void testWideString() {
	}


	static enResult getHandler (oValue& v, const iColumn*, const iRow*, enColumnType type, GetFlags) {
		if (type != ctypeString) {
			char ret [2];
			v = new Value_int( v->castStaticObject<Value_string>()->getString().a_str()[0] - 'A');
		}
		return DT::success;
	}
 
	static enResult setHandler (oValue& v, const iColumn*, iRow*, SetFlags) {
		if (v->getType() == ctypeInt) {
			char ret [2];
			ret[1] = 0;
			ret[0] = 'A' + v->castStaticObject<Value_int>()->getInt();
			v = new Value_string(ret); // podmieniamy typ...
		}
		return DT::success;
	}

	void testHandlers() {

		DataTable dt;
		Column* col = dt.setColumn(colString, ctypeString)->castStaticObject<Column>();

		col->getHandler = getHandler;
		col->setHandler = setHandler;

		DataRow* row = dt.addRow();
		col->setString(row, "C");

		CPPUNIT_ASSERT_EQUAL( (int)2, col->getInt(row) );
		CPPUNIT_ASSERT_EQUAL( String("C"), col->getString(row) );

		col->setInt(row, 5);
		CPPUNIT_ASSERT_EQUAL( (int)5, col->getInt(row) );
		CPPUNIT_ASSERT_EQUAL( (__int64)5, col->getInt64(row) );
		CPPUNIT_ASSERT_EQUAL( String("F"), col->getString(row) );

	}

	static int _triggerTest;

	static void preTrigger(const Value* v, const iColumn* col, iRow* row, SetFlags) {
		_triggerTest++;
		CPPUNIT_ASSERT_EQUAL( String("A"), col->getString(row) );
		CPPUNIT_ASSERT_EQUAL( String("B"), v->castStaticObject<const Value_string>()->getString() );
	}

	static void postTrigger(const iColumn* col, iRow* row, SetFlags) {
		_triggerTest++;
		CPPUNIT_ASSERT_EQUAL( String("B"), col->getString(row) );
	}

	void testTriggers() {

		_triggerTest = 0;

		DataTable dt;
		Column* col = dt.setColumn(colString, ctypeString)->castStaticObject<Column>();
		
		DataRow* row = dt.addRow();
		col->setString(row, "A");
		col->preTrigger.connect(&TestTable::preTrigger);
		col->postTrigger.connect(&TestTable::postTrigger);

		col->setString(row, "B");
		CPPUNIT_ASSERT_EQUAL( String("B"), col->getString(row) );
		CPPUNIT_ASSERT_EQUAL( (int)2, _triggerTest );

	}

};


int TestTable::_triggerTest;


CPPUNIT_TEST_SUITE_REGISTRATION( TestTable );


