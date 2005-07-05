#include <stdafx.h>
#include <cppunit/extensions/HelperMacros.h>
#include "..\DataTable.h"
#include "..\FileBin.h"
#include <Stamina\MD5.h>
#include <Stamina\Helpers.h>

using namespace Stamina;
using namespace Stamina::DT;

namespace Stamina { namespace DT {

class ConsoleProgress {
public:
	ConsoleProgress(unsigned int from, unsigned int to, unsigned int width = 50) {
//		_from = from;
		_to = to - from;
		_pos = 0;
		_width = width;
		_bars = 0;
	}
	void drawBar() {
		for (int i=0; i < _width; i++) {
			std::cout << ".";
		}
	}
	void doStep(unsigned int offset = 1) {
		_pos += offset;
		unsigned int newBars = ((float)_pos / _to) * _width;
		while (_bars < newBars) {
			std::cout << "#";
			_bars++;
		}
	}
private:
	unsigned int _from, _to;
	unsigned int _pos;
	unsigned int _width;
	unsigned int _bars;

};


class TestFileBin : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestFileBin );
  
	CPPUNIT_TEST( testSave );
	CPPUNIT_TEST( testLoad );
	CPPUNIT_TEST( testAppend );
	CPPUNIT_TEST( testSetErased );
	CPPUNIT_TEST( testBadPassword );
	CPPUNIT_TEST( testStress );

    CPPUNIT_TEST_SUITE_END();

protected:

	ColumnsDesc _cols;
	ColumnsDesc _colsNoExtra;
	static const tColId colInt = 1;
	static const tColId colString = 2;
	static const tColId colIntDef = 3;
	static const tColId colStringDef = 4;
	static const tColId col64 = 5;
	static const tColId col64Def = 6;
	static const tColId colBin = 7;
	static const tColId colBinDef = 8;
	static const tColId colExtra = 9;
  
	tRowId row1;
	tRowId row2;
	tRowId row3;

	std::string testString1, testStringDef;
	std::string testString2;
	std::string testString3;

	std::string password;
	bool cryptAll;


	int testInt, testIntDef;
	__int64 test64, test64Def;
	TypeBin testBin, testBinDef;

	char buffer [50];
	TypeBin binBuffer;

public:
	void setUp() {
		cryptAll = false;
		password = "";

		binBuffer.size = 50;
		binBuffer.buff = buffer;

		row1 = DataTable::flagId(1);
		row2 = DataTable::flagId(2);
		row3 = DataTable::flagId(3);

		testInt = 100;
		testIntDef = 1000;
		test64 = 0xFF00000000;
		test64Def = 0xAA00000001;
		testBin.size = 10;
		testBin.buff = "1234567890";
		testBinDef.size = 5;
		testBinDef.buff = "12345";
		testString1 = "sadhgasd gadshagd asdgsh dgsajhag dshgd";
		testString2 = "4u535873658653875637856345876345g dshgd";
		testString3 = "AAHSGAJSGASJGASJGJSGASGJSGASHGSHAGSHGSAG";
		testStringDef = "DEFAULT";

		_colsNoExtra.setColumn(colInt, ctypeInt , 0, "Int");
		_colsNoExtra.setColumn(colIntDef, ctypeInt | cflagXor, (DataEntry) testIntDef, "IntDef");
		_colsNoExtra.setColumn(1000, ctypeInt | cflagDontSave);
		_colsNoExtra.setColumn(colString, ctypeString, 0, "String");
		_colsNoExtra.setColumn(colStringDef, ctypeString | cflagXor, (void*)testStringDef.c_str(), "StringDef");
		_colsNoExtra.setColumn(1001, ctypeString | cflagDontSave);
		_colsNoExtra.setColumn(col64, ctype64, 0, "64");
		_colsNoExtra.setColumn(col64Def, ctype64 | cflagXor, &test64Def, "64Def");
		_colsNoExtra.setColumn(1002, ctype64 | cflagDontSave);
		_colsNoExtra.setColumn(colBin, ctypeBin, 0, "Bin");
		_colsNoExtra.setColumn(colBinDef, ctypeBin | cflagXor, &testBinDef, "BinDef");
		_colsNoExtra.setColumn(1003, ctypeBin | cflagDontSave);

		_cols.join(_colsNoExtra, false);
		_cols.setColumn(colExtra, ctypeString, (DataEntry)"!!!!!!EXTRA!!!!!!");
	}
	void tearDown() {
	}

protected:

	std::string getFileName(const std::string& name) {
		return "TestFileBin__" + name + "_p-" + password + ".dtb";
	}

	void createFile(const std::string& name) {
		DataTable dt;
		dt.mergeColumns(_cols);

		dt.setPassword(password);

		dt.addRow(row1);
		dt.addRow(row2);
		dt.addRow(row3);

		dt.setInt(row1, colInt, testInt);
		dt.set64(row1, col64, test64);
		dt.setBin(row1, colBin, testBin);
		dt.setStr(row1, colString, testString1);
		dt.setStr(row1, colStringDef, testString2);
		dt.setStr(row2, colString, testString2);
		dt.setStr(row3, colString, testString3);

		FileBin fb;
		fb.assign(dt);
		fb.setFileFlag(FileBin::fflagCryptAll, cryptAll);
		fb.save(getFileName(name));
	}


	void testSave() {
		DataTable dt;
		dt.mergeColumns(_cols);

		dt.setPassword(password);
		dt.addRow(row1);
		dt.addRow(row2);

		FileBin fb;
		fb.assign(dt);


		fb.setColumns( dt.getColumns() );
		CPPUNIT_ASSERT( fb.getColumns().getColCount() > 0 );

		fb.setFileFlag(FileBin::fflagCryptAll, true);

		fb.open(getFileName("testSave") , fileWrite);
		CPPUNIT_ASSERT( fb.isOpened() );
		CPPUNIT_ASSERT( fb.isCreatingNewFile() );
		CPPUNIT_ASSERT( fb.isReadable() == false );
		CPPUNIT_ASSERT( fb.isWriteFailed() == false );

		fb.writeHeader();
		CPPUNIT_ASSERT( fb._xorDigest.empty() == false );
		CPPUNIT_ASSERT( fb._xorSalt != 0 );
		CPPUNIT_ASSERT( fb._passwordDigest.empty() == false );
		CPPUNIT_ASSERT( fb._passwordSalt != 0 );
		CPPUNIT_ASSERT( fb.isWriteFailed() == false );
		fb.writeDescriptor();
		CPPUNIT_ASSERT( fb.isWriteFailed() == false );

		fb.writeRow(row1);
		fb.writeRow(row2);

		fb.close();
		CPPUNIT_ASSERT( fb.isWriteFailed() == false );
	}


	void testLoad() {
		createFile("testLoad");

		DataTable dt;
		tColId colString2 = 100;
		dt.setColumn(colString2, ctypeString | cflagXor, (DataEntry) testStringDef.c_str());

		dt.mergeColumns(_cols);

		dt.setPassword(password);

		FileBin fbl;
		fbl.assign(dt);
		CPPUNIT_ASSERT( fbl.load(getFileName("testLoad")) == success );

		CPPUNIT_ASSERT( fbl.hasFileFlag(FileBin::fflagCryptAll) == cryptAll );

		CPPUNIT_ASSERT_EQUAL( testString1, dt.getStr(row1, colString) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt.getStr(row1, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt.getStr(row2, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt.getStr(row2, colString2) );
		CPPUNIT_ASSERT_EQUAL( testInt, dt.getInt(row1, colInt) );
		CPPUNIT_ASSERT_EQUAL( test64, dt.get64(row1, col64) );
		CPPUNIT_ASSERT( dt.getBin(row1, colBin, binBuffer) == testBin );
		CPPUNIT_ASSERT_EQUAL( testIntDef, dt.getInt(row1, colIntDef) );
		CPPUNIT_ASSERT_EQUAL( test64Def, dt.get64(row1, col64Def) );
		CPPUNIT_ASSERT( dt.getBin(row1, colBinDef, binBuffer) == testBinDef );

		dt.setStr(row1, colString, testString2);
		dt.setStr(row2, colString2, testString2);

		FileBin fbs;
		fbs.assign(dt);
		fbs.setFileFlag(FileBin::fflagCryptAll, cryptAll);
		CPPUNIT_ASSERT( fbs.save(getFileName("testLoad")) == success );

		DataTable dt2;
		dt2.setPassword(password);

		FileBin fbl2;
		fbl2.assign(dt2);
		CPPUNIT_ASSERT( fbl2.loadAll(getFileName("testLoad")) == success );

		CPPUNIT_ASSERT( fbl2.hasFileFlag(FileBin::fflagCryptAll) == cryptAll );

		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getStr(row1, colString) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getStr(row1, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt2.getStr(row2, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getStr(row2, colString2) );

	}


	void testAppend() {
		createFile("testAppend");

		DataTable dt;
		dt.setPassword(password);
		tColId colString2 = 100;
		dt.setColumn(colString2, ctypeString | cflagXor, (DataEntry) testStringDef.c_str());
		dt.mergeColumns(_colsNoExtra);

		tRowId rowA = DataTable::flagId( 100 );
		tRowId rowB = DataTable::flagId( 101 );

		dt.addRow(rowA);
		dt.addRow(rowB);

		dt.setStr(rowA, colString, testString1);
		dt.setStr(rowB, colString, testString2);
		dt.setStr(rowA, colString2, testString3);

		FileBin fba;
		fba.assign(dt);
		CPPUNIT_ASSERT( fba.append(getFileName("testAppend")) == success );

		CPPUNIT_ASSERT( fba.hasFileFlag(FileBin::fflagCryptAll) == cryptAll );

		DataTable dt2;
		dt2.setPassword(password);
		FileBin fbl2;
		fbl2.assign(dt2);
		CPPUNIT_ASSERT( fbl2.loadAll(getFileName("testAppend")) == success );

		rowA = 3;
		rowB = 4;

		CPPUNIT_ASSERT( fbl2.hasFileFlag(FileBin::fflagCryptAll) == cryptAll );
		CPPUNIT_ASSERT_EQUAL( (unsigned int)5, dt2.getRowCount() );

		CPPUNIT_ASSERT_EQUAL( testString1, dt2.getStr(row1, colString) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getStr(row1, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt2.getStr(row2, colStringDef) );

		CPPUNIT_ASSERT( dt2.getColumns().getColumn(colString2).empty() );

		CPPUNIT_ASSERT_EQUAL( testString1, dt2.getStr(rowA, colString) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getStr(rowB, colString) );

		CPPUNIT_ASSERT_EQUAL( testInt, dt2.getInt(row1, colInt) );
		CPPUNIT_ASSERT( dt2.getBin(row1, colBin, binBuffer) == testBin );

		CPPUNIT_ASSERT_EQUAL( testIntDef, dt2.getInt(rowA, colIntDef) );
		CPPUNIT_ASSERT( dt2.getBin(rowA, colBinDef, binBuffer) == testBinDef );

	}


	void testSetErased() {
		createFile("testErased");

		DataTable dt1;
		dt1.setPassword(password);
		FileBin fb1;
		fb1.assign(dt1);
		fb1.open(getFileName("testErased"), fileReadWrite);
		fb1.readDescriptor();
		fb1.readRow( dt1.addRow() ); // 0
		//fb1.findNextRow(); // 1
		fb1.setErasedRow();
        fb1.close();

		DataTable dt2;
		FileBin fb2;
		dt2.setPassword(password);
		fb2.assign(dt2);
		fb2.load(getFileName("testErased"));

		CPPUNIT_ASSERT_EQUAL( testString1, dt2.getStr(0, colString) );
		// jako drugi powinien zl¹dowaæ poprzedni trzeci...
		CPPUNIT_ASSERT_EQUAL( testString3, dt2.getStr(1, colString) );
		CPPUNIT_ASSERT_EQUAL( (unsigned int)2, dt2.getRowCount() );
		
	}

	void testBadPassword() {
		createFile("testPassword");

		DataTable dt;
		FileBin fb;
		dt.setPassword("z³ehas³o");
		fb.assign(dt);
		enResult result = fb.load(getFileName("testPassword"));
		CPPUNIT_ASSERT_EQUAL( errNotAuthenticated, result );
		CPPUNIT_ASSERT( dt.getRowCount() == 0 );

	}

	void testStress() {
		const int stressCount = 1000;
		const int appendCount = 2;
		int i;
		tRowId startRow = rowIdMax - 100;
		{
			DataTable dt1;
			FileBin fb1;
			dt1.setPassword(password);
			dt1.mergeColumns(_cols);
			fb1.assign(dt1);
			ConsoleProgress cp(0, stressCount);
			std::cout << std::endl << "Zape³niam tablicê save" << std::endl;
			cp.drawBar();
			std::cout << std::endl;
			for (i = 0; i < stressCount; i++) {
				tRowId row = dt1.addRow(i == 0 ? startRow : rowNotFound);
				dt1.setInt(row, colInt, i);
				dt1.setStr(row, colString, inttostr(i));
				cp.doStep();
			}
			fb1.save(getFileName("testStress"));
		}
		// append
		{
			DataTable dt2;
			FileBin fb2;
			dt2.setPassword(password);
			dt2.mergeColumns(_cols);
			fb2.assign(dt2);
			ConsoleProgress cp(0, stressCount);
			std::cout << std::endl << "Zape³niam tablicê append" << std::endl;
			cp.drawBar();
			std::cout << std::endl;
			for (; i < 2 * stressCount; i++) {
				tRowId row = dt2.addRow();
				dt2.setInt(row, colInt, i);
				dt2.setStr(row, colString, inttostr(i));
				cp.doStep();
			}
			ConsoleProgress cp2(0, appendCount);
			std::cout << std::endl << "Dopisujê rekordy" << std::endl;
			cp2.drawBar();
			std::cout << std::endl;
			for (int app = 0; app < appendCount; app++) {
				fb2.append(getFileName("testStress"));
				cp2.doStep();
			}
		}
		// load
		{
			DataTable dt3;
			FileBin fb3;
			dt3.setPassword(password);
			fb3.assign(dt3);
			fb3.open(getFileName("testStress"), fileRead);
			fb3.readDescriptor();
			fb3.mergeLoadedColumns();
			CPPUNIT_ASSERT_EQUAL((unsigned int) stressCount * (appendCount + 1), fb3.getStoredRowsCount() );
			int failed = rowNotFound;
			int read = rowNotFound;
			tRowId row = dt3.addRow();
			int checkFrom = 0;
			int checkTo = stressCount;
			ConsoleProgress cp(0, fb3.getStoredRowsCount());
			std::cout << std::endl << "Sprawdzanie" << std::endl;
			cp.drawBar();
			std::cout << std::endl;
			for (int app = 0; app <= appendCount; app++) {
				for (i = checkFrom; i < checkTo; i++) {
					fb3.readRow(row, false);
					cp.doStep();
					read = dt3.getInt(row, colInt);
					if (dt3.getInt(row, colInt) != i || dt3.getStr(row, colString) != inttostr(i) || dt3.getStr(row, colStringDef) != testStringDef) {
						failed = i;
						break;
					}
				}
				checkFrom = stressCount;
				checkTo = stressCount * 2;
			}
			CPPUNIT_ASSERT_EQUAL( (int) -1, failed );
			CPPUNIT_ASSERT_EQUAL( (int) (2 * stressCount - 1), read );
			fb3.close();
		}

	}

	void testOldLoad() {
	}

	void testOldAppend() {
	}

	void testTemporary() {
	}

	void testBackup() {
	}

	void testFileLockers() {
	}

};


class TestFileBinPass : public TestFileBin
{
	CPPUNIT_TEST_SUITE( TestFileBinPass );
  
	CPPUNIT_TEST( testSave );
	CPPUNIT_TEST( testLoad );
	CPPUNIT_TEST( testAppend );
	CPPUNIT_TEST( testSetErased );
	CPPUNIT_TEST( testBadPassword );
	CPPUNIT_TEST( testStress );

    CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {
		TestFileBin::setUp();
		password = "abc";
	}
};

class TestFileBinCryptAll : public TestFileBin
{
	CPPUNIT_TEST_SUITE( TestFileBinCryptAll );
  
	CPPUNIT_TEST( testSave );
	CPPUNIT_TEST( testLoad );
	CPPUNIT_TEST( testAppend );
	CPPUNIT_TEST( testSetErased );
	CPPUNIT_TEST( testBadPassword );
	CPPUNIT_TEST( testStress );


    CPPUNIT_TEST_SUITE_END();
public:
	void setUp() {
		TestFileBin::setUp();
		password = "cryptAll";
		cryptAll = true;
	}

};


} }

CPPUNIT_TEST_SUITE_REGISTRATION( TestFileBin );
CPPUNIT_TEST_SUITE_REGISTRATION( TestFileBinPass );
CPPUNIT_TEST_SUITE_REGISTRATION( TestFileBinCryptAll );


