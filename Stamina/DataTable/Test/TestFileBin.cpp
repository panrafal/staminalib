#include <stdafx.h>
#include <list>
#include <ostream>
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina/VersionControl.h>
#include "..\DataTable.h"
#include "..\FileBin.h"
#include "..\InterfaceBasic.h"
#include <Stamina\MD5.h>
#include <Stamina\Helpers.h>
#include <Stamina\WideChar.h>

using namespace Stamina;
using namespace Stamina::DT;

using std::string;
using std::cout;
using std::endl;

#include <Stamina\Console.h>
#include <Stamina\FindFile.h>

namespace Stamina { namespace DT {



class TestFileBin : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestFileBin );
  
	CPPUNIT_TEST( testSave );
	CPPUNIT_TEST( testLoad );
	CPPUNIT_TEST( testAppend );
	CPPUNIT_TEST( testSetErased );
	CPPUNIT_TEST( testBadPassword );
//	CPPUNIT_TEST( testStress );
	CPPUNIT_TEST( testOldLoad );
	CPPUNIT_TEST( testOldAppend );
	CPPUNIT_TEST( testChangeType );
	CPPUNIT_TEST( testTemporary );
	CPPUNIT_TEST( testBackup );
	CPPUNIT_TEST( testBackupCleanupDirectory );
	CPPUNIT_TEST( testBackupCleanupFile );
	CPPUNIT_TEST( testInterface );

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
	static const tColId colWide = 10;
	static const tColId colWideDef = 11;
  
	tRowId row1;
	tRowId row2;
	tRowId row3;

	String testString1, testStringDef;
	String testString2;
	String testString3;
	String testWide, testWideDef;

	String param1;
	String param2;
	String paramValue1;
	String paramValue2;

	std::string password;
	bool cryptAll;


	int testInt, testIntDef;
	__int64 test64, test64Def;
	ByteBuffer testBin, testBinDef;

	char buffer [50];
	ByteBuffer binBuffer;

public:

	TestFileBin() {
		FindFile ff(".\\TestFileBin__test*");
		deleteFiles( ff.makeList() );

		cryptAll = false;
		password = "";

		binBuffer.assign((unsigned char*)buffer, 50);

		row1 = DataTable::flagId(1);
		row2 = DataTable::flagId(2);
		row3 = DataTable::flagId(3);

		testInt = 100;
		testIntDef = 1000;
		test64 = 0xFF00000000;
		test64Def = 0xAA00000001;
		testBin.assign((unsigned char*)"1234567890", 10);
		testBinDef.assign((unsigned char*)"12345", 5);
		testString1 = "sadhgasd gadshagd asdgsh dgsajhag dshgd";
		testString2 = "4u535873658653875637856345876345g dshgd";
		testString3 = "AAHSGAJSGASJGASJGJSGASGJSGASHGSHAGSHGSAG";
		testStringDef = "DEFAULT";

		param1 = "Param1";
		param2 = L"Param2";
		paramValue1 = "Value1¹æ";
		paramValue2 = L"Value2¹æ";

		testWide = L"¥Œê¹œ¹ê¹œ¹ó³ñ¿ŸSDSAJDHSDY WSADSJDH ";
		testWideDef = L"D¹FAULT";

		_colsNoExtra.setColumn(colInt, ctypeInt , "Int");
		_colsNoExtra.setColumn(colIntDef, ctypeInt | cflagXor, "IntDef")->setInt(rowDefault, testIntDef);
		_colsNoExtra.setColumn(1000, ctypeInt | cflagDontSave);
		_colsNoExtra.setColumn(colString, ctypeString, "String");
		_colsNoExtra.setColumn(colStringDef, ctypeString | cflagXor, "StringDef")->setString(rowDefault, testStringDef);
		_colsNoExtra.setColumn(colWide, ctypeString, "Wide");
		_colsNoExtra.setColumn(colWideDef, ctypeString | cflagXor, "WideDef")->setString(rowDefault, testWideDef);
		_colsNoExtra.setColumn(1001, ctypeString | cflagDontSave);
		_colsNoExtra.setColumn(col64, ctype64, "64");
		_colsNoExtra.setColumn(col64Def, ctype64 | cflagXor, "64Def")->setInt64(rowDefault, test64Def);
		_colsNoExtra.setColumn(1002, ctype64 | cflagDontSave);
		_colsNoExtra.setColumn(colBin, ctypeBin, "Bin");
		_colsNoExtra.setColumn(colBinDef, ctypeBin | cflagXor, "BinDef")->setBin(rowDefault, testBinDef);
		_colsNoExtra.setColumn(1003, ctypeBin | cflagDontSave);

		_cols.join(_colsNoExtra, false);
		_cols.setColumn(colExtra, ctypeString)->setString(rowDefault, L"!!!!!!EXTRA!!!!!!");
	}
	void setUp() {
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
		dt.setString(row1, colString, testString1);
		dt.setString(row1, colStringDef, testString2);
		dt.setString(row1, colWide, testWide);
		dt.setString(row2, colString, testString2);
		dt.setString(row3, colString, testString3);

		dt.setParam(param1, paramValue1);
		dt.setParam(param2, paramValue2);

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
		dt.setColumn(colString2, ctypeString | cflagXor)->setString(rowDefault, testStringDef);

		dt.mergeColumns(_colsNoExtra);

		dt.setPassword(password);

		FileBin fbl;
		fbl.assign(dt);
		CPPUNIT_ASSERT( fbl.load(getFileName("testLoad")) == success );

		CPPUNIT_ASSERT( dt.getColumns().getColumn(colExtra)->hasFlag(cflagIsLoaded) );
		CPPUNIT_ASSERT( dt.getColumns().getColumn(colString)->hasFlag(cflagIsDefined) );

		CPPUNIT_ASSERT( fbl.hasFileFlag(FileBin::fflagCryptAll) == cryptAll );

		CPPUNIT_ASSERT_EQUAL( testString1, dt.getString(row1, colString) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt.getString(row1, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt.getString(row2, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt.getString(row2, colString2) );
		CPPUNIT_ASSERT_EQUAL( testWide, dt.getString(row1, colWide) );
		CPPUNIT_ASSERT_EQUAL( testWideDef, dt.getString(row1, colWideDef) );
		CPPUNIT_ASSERT_EQUAL( testInt, dt.getInt(row1, colInt) );
		CPPUNIT_ASSERT_EQUAL( test64, dt.get64(row1, col64) );
		CPPUNIT_ASSERT( dt.getBin(row1, colBin) == testBin );
		CPPUNIT_ASSERT_EQUAL( testIntDef, dt.getInt(row1, colIntDef) );
		CPPUNIT_ASSERT_EQUAL( test64Def, dt.get64(row1, col64Def) );
		CPPUNIT_ASSERT( dt.getBin(row1, colBinDef) == testBinDef );

		CPPUNIT_ASSERT_EQUAL( paramValue1, dt.getParam(param1) );
		CPPUNIT_ASSERT_EQUAL( paramValue2, dt.getParam(param2) );

		dt.setString(row1, colString, testString2);
		dt.setString(row2, colString2, testString2);
		dt.setString(row1, colWide, testString1);

		dt.setParam(param1, paramValue2);

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

		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getString(row1, colString) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getString(row1, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt2.getString(row2, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getString(row2, colString2) );
		CPPUNIT_ASSERT_EQUAL( testString1, dt2.getString(row1, colWide) );

		CPPUNIT_ASSERT_EQUAL( paramValue2, dt.getParam(param1) );

	}


	void testAppend() {
		createFile("testAppend");

		DataTable dt;
		dt.setPassword(password);
		tColId colString2 = 100;
		dt.setColumn(colString2, ctypeString | cflagXor)->setString(rowDefault, testStringDef);
		dt.mergeColumns(_colsNoExtra);

		tRowId rowA = DataTable::flagId( 100 );
		tRowId rowB = DataTable::flagId( 101 );

		dt.addRow(rowA);
		dt.addRow(rowB);

		dt.setString(rowA, colString, testString1);
		dt.setString(rowB, colString, testString2);
		dt.setString(rowA, colString2, testString3);

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

		CPPUNIT_ASSERT_EQUAL( testString1, dt2.getString(row1, colString) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getString(row1, colStringDef) );
		CPPUNIT_ASSERT_EQUAL( testStringDef, dt2.getString(row2, colStringDef) );

		CPPUNIT_ASSERT( dt2.getColumns().getColumn(colString2)->isUndefined() );

		CPPUNIT_ASSERT_EQUAL( testString1, dt2.getString(rowA, colString) );
		CPPUNIT_ASSERT_EQUAL( testString2, dt2.getString(rowB, colString) );


		CPPUNIT_ASSERT_EQUAL( testWide, dt2.getString(row1, colWide) );
		CPPUNIT_ASSERT_EQUAL( testInt, dt2.getInt(row1, colInt) );
		CPPUNIT_ASSERT( dt2.getBin(row1, colBin) == testBin );

		CPPUNIT_ASSERT_EQUAL( testWideDef, dt2.getString(rowA, colWideDef) );
		CPPUNIT_ASSERT_EQUAL( testIntDef, dt2.getInt(rowA, colIntDef) );
		CPPUNIT_ASSERT( dt2.getBin(rowA, colBinDef) == testBinDef );

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

		CPPUNIT_ASSERT_EQUAL( testString1, dt2.getString(0, colString) );
		// jako drugi powinien zl¹dowaæ poprzedni trzeci...
		CPPUNIT_ASSERT_EQUAL( testString3, dt2.getString(1, colString) );
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
		const int stressCount = 2000;
		const int appendCount = 5;
		int i;
		tRowId startRow = rowIdMax - 100;
		{
			DataTable dt1;
			FileBin fb1;
			dt1.setPassword(password);
			dt1.mergeColumns(_cols);
			fb1.assign(dt1);
			fb1.setFileFlag(FileBin::fflagCryptAll, cryptAll);
			ConsoleProgress cp(0, stressCount);
			std::cout << std::endl << "Zape³niam tablicê save" << std::endl;
			cp.drawBar();
			std::cout << std::endl;
			for (i = 0; i < stressCount; i++) {
				tRowId row = dt1.addRow(i == 0 ? startRow : rowNotFound);
				dt1.setInt(row, colInt, i);
				dt1.setString(row, colString, inttostr(i));
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
				dt2.setString(row, colString, inttostr(i));
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
					if (dt3.getInt(row, colInt) != i || dt3.getString(row, colString) != inttostr(i) || dt3.getString(row, colStringDef) != testStringDef || dt3.getString(row, colWideDef) != testWideDef) {
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
		{
			DataTable dt;
			dt.setXor1Key("\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84");
			FileBin fb(dt);
			CPPUNIT_ASSERT( fb.loadAll("TestFileBinOld.dtb") == success );
			CPPUNIT_ASSERT_EQUAL( (unsigned int) 3, dt.getRowCount() );

			CPPUNIT_ASSERT_EQUAL( String("Kontakt 0"), dt.getString(0, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( String("Imiê 0"), dt.getString(0, dt.getColumnId("Name")) );
			CPPUNIT_ASSERT_EQUAL( String("Kontakt 2"), dt.getString(2, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( String("Imiê 2"), dt.getString(2, dt.getColumnId("Name")) );

			FileBin fb2(dt);
			fb2.setFileFlag(FileBin::fflagCryptAll, cryptAll);
			if (!password.empty()) {
				dt.setPassword(password);
			}
			// powinno siê zapisaæ tak samo...
			fb2.save(getFileName("testOldSave"), saveOldCryptVersion);

		}

		DataTable dt3;
		if (!password.empty()) {
			dt3.setPassword(password);
		}
		dt3.setXor1Key("\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84");
		FileBin fb3(dt3);
		fb3.load(getFileName("testOldSave"));
		
		CPPUNIT_ASSERT_EQUAL( (unsigned int) 3, dt3.getRowCount() );

		CPPUNIT_ASSERT_EQUAL( String("Kontakt 0"), dt3.getString(0, dt3.getColumnId("Display")) );
		CPPUNIT_ASSERT_EQUAL( String("Imiê 0"), dt3.getString(0, dt3.getColumnId("Name")) );
		CPPUNIT_ASSERT_EQUAL( String("Kontakt 2"), dt3.getString(2, dt3.getColumnId("Display")) );
		CPPUNIT_ASSERT_EQUAL( String("Imiê 2"), dt3.getString(2, dt3.getColumnId("Name")) );

	}

	void testOldAppend() {
		unlink(getFileName("testOldAppend").c_str());
        CopyFile("TestFileBinOld.dtb", getFileName("testOldAppend").c_str() , false);	
		DataTable dt;
		dt.setXor1Key("\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84");
		{
			FileBin fb(dt);
			// wgrywa wszystkie bebechy
			fb.loadAll(getFileName("testOldAppend"));
		}
		// zmieniamy minimalnie...
		dt.setString(1, dt.getColumnId("Display"), "Kontakt zmieniony");
		dt.setString(1, dt.getColumnId("Name"), "Imiê zmienione");
		tRowId row = dt.insertRow(0);
		dt.setString(row, dt.getColumnId("Display"), "Kontakt dodany");
		dt.setString(row, dt.getColumnId("Name"), "Imiê dodane");
		{
			FileBin fb(dt);
			fb.append(getFileName("testOldAppend"));
		}
		{
			DataTable dt;
			dt.setXor1Key("\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84");
			FileBin fb(dt);
			fb.load(getFileName("testOldAppend"));
			CPPUNIT_ASSERT_EQUAL( (unsigned int) 7, dt.getRowCount() );

			CPPUNIT_ASSERT_EQUAL( String("Kontakt 0"), dt.getString(0, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( String("Imiê 0"), dt.getString(4, dt.getColumnId("Name")) );
			CPPUNIT_ASSERT_EQUAL( String("Kontakt 2"), dt.getString(2, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( String("Imiê 2"), dt.getString(6, dt.getColumnId("Name")) );
			CPPUNIT_ASSERT_EQUAL( String("Kontakt dodany"), dt.getString(3, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( String("Imiê zmienione"), dt.getString(5, dt.getColumnId("Name")) );
		}
	}

	void testChangeType() {
		createFile("testChangeType");

		DataTable dt;
		dt.setPassword(password);
		FileBin fb(dt);
		dt.setColumn(colString, ctypeInt)->setInt(rowDefault, testIntDef);
		fb.load(getFileName("testChangeType"));
		// powinien zignorowaæ zawartoœæ pliku i u¿yæ wartoœci domyœlnej...
		CPPUNIT_ASSERT_EQUAL( testIntDef, dt.getInt(0, colString) );
	}

	void testTemporary() {
		createFile("testTemporary");

		DataTable dt;
		dt.setPassword(password);
		FileBin fb(dt);
		fb.useTempFile = true;
		fb.loadAll(getFileName("testTemporary"));
		CPPUNIT_ASSERT_EQUAL(testString1, dt.getString(row1, colString));
		// skoro jest za³adowane próbujemy nieudanego zapisu
		dt.setString(row1, colString, testString2);
		fb.open(getFileName("testTemporary"), fileWrite);
		fb.setWriteFailed(true);
		CPPUNIT_ASSERT(fb._temp_enabled == true);
		CPPUNIT_ASSERT(fb._temp_fileName != "");
		CPPUNIT_ASSERT(fb.isUsingTemp() == true);
		CPPUNIT_ASSERT(fb.getFileName() != fb.getOpenedFileName());
		fb.writeHeader();
		fb.writeDescriptor();
		fb.writeRow(row1);
		try {
			fb.close();
			CPPUNIT_ASSERT(false);
		} catch (...) {
		}

		fb.loadAll(getFileName("testTemporary"));
		CPPUNIT_ASSERT_EQUAL(testString1, dt.getString(row1, colString));
		dt.setString(row1, colString, testString2);
		fb.useTempFile = true;
		fb.save();
		fb.loadAll();
		CPPUNIT_ASSERT_EQUAL(testString2, dt.getString(row1, colString));
	}

	/* 
	- czy .bak jest tworzony przy zapisie bez tempa
	- czy pomija tworzenie pliku .bak je¿ przed chwil¹ jeden zrobi³
	- czy .bak jest tworzony przy zapisie z temp'em
	- wyszukiwanie ostatniego .bak'a
	- przywracanie .bak'ów
	*/
	void testBackup() {
		createFile("testBackup");
		std::string filename = this->getFileName("testBackup");

		DataTable dt;
		dt.setPassword(password);
		FileBin fb(dt);
		fb.loadAll(filename);
		dt.setString(row1, colString, testString2);
		fb.useTempFile = false;
		fb.makeBackups = true;
		// backup bez tempa
		fb.save(filename);
		CPPUNIT_ASSERT(dt.getTimeLastBackup().empty() == false);
		std::string backup = fb.findLastBackupFile();
		CPPUNIT_ASSERT(fb.findLastBackupDate().empty() == false);
		CPPUNIT_ASSERT(backup.empty() == false);
		CPPUNIT_ASSERT(backup.find("testBackup"));
		DeleteFile(backup.c_str());
		// nie powinien zrobiæ backupu
		fb.backupFile(false);
		CPPUNIT_ASSERT(fb.findLastBackupDate().empty() == true);
		dt.setTimeLastBackup(Time64());
		// powinien zrobiæ backup
		fb.backupFile(false);
		CPPUNIT_ASSERT(fb.findLastBackupDate().empty() == false);
		// backup z tempem
		DeleteFile(fb.findLastBackupFile().c_str());
		dt.setTimeLastBackup(Time64());
		fb.useTempFile = true;
		fb.save(filename);
		backup = fb.findLastBackupFile();
		CPPUNIT_ASSERT(backup.empty() == false);
		// sprawdzamy wyszukiwanie najnowszych...
		CopyFile(filename.c_str(), (filename + Time64(Time64(true) - 120).strftime(".%Y-%m-%d %H-%M-%S.bak")).c_str(), false);
		CPPUNIT_ASSERT_EQUAL(String(backup), fb.findLastBackupFile());

		// przywracanie
		createFile("testBackup");
		fb.restoreLastBackup();
		fb.loadAll();
		CPPUNIT_ASSERT_EQUAL(testString2, dt.getString(row1, colString));

	}

	/*
	- czy usuwa prawid³owe pliki
	- czy dzia³a prawid³owo tryb plik/katalog
	*/
	void testBackupCleanup(bool all) {
		typedef std::list<Date64> tList;
		tList keep;
		tList remove;

		keep.push_back(Time64(true) - (30 * 24 * 60 * 60) - 10); // sprzed miesiaca
		keep.push_back(Time64(true) - (7 * 24 * 60 * 60)); // sprzed tygodnia
		keep.push_back(Time64(true) - (24 * 60 * 60) - 550); // z wczoraj
		keep.push_back(Time64(true) - (6 * 60 * 60) - 250); // sprzed 6 godzin
		keep.push_back(Time64(true) - 10); // najnowsze

		remove.push_back(Time64(true) - (60 * 24 * 60 * 60));
		remove.push_back(Time64(true) - (365 * 24 * 60 * 60));
		remove.push_back(Time64(true) - (30 * 24 * 60 * 60) - 11);
		remove.push_back(Time64(true) - (8 * 24 * 60 * 60)); // sprzed tygodnia
		remove.push_back(Time64(true) - (14 * 24 * 60 * 60)); // sprzed tygodnia
		remove.push_back(Time64(true) - (29 * 24 * 60 * 60)); // sprzed tygodnia
		remove.push_back(Time64(true) - (25 * 60 * 60)); // z wczoraj
		remove.push_back(Time64(true) - (48 * 60 * 60)); // z wczoraj
		remove.push_back(Time64(true) - (7 * 60 * 60)); // sprzed 6 godzin
		remove.push_back(Time64(true) - (12 * 60 * 60)); // sprzed 6 godzin
		remove.push_back(Time64(true) - 1000); // najnowsze

		std::string format = getFileName("testCleanup") + ".%Y-%m-%d %H-%M-%S.bak";

		for (tList::iterator it = keep.begin(); it != keep.end(); it++) {
			CopyFile("TestFileBinOld.dtb", it->strftime(format.c_str()).c_str() , false);
		}
		for (tList::iterator it = remove.begin(); it != remove.end(); it++) {
			CopyFile("TestFileBinOld.dtb", it->strftime(format.c_str()).c_str() , false);
		}

		std::string testAll1 = getFileName("testCleanupAll") + ".2001-06-10 12-00-00.bak";
		std::string testAll2 = getFileName("testCleanupAll") + ".2001-06-09 12-00-00.bak";
		CopyFile("TestFileBinOld.dtb", testAll1.c_str(), false);
		CopyFile("TestFileBinOld.dtb", testAll2.c_str(), false);

		FileBin::cleanupBackups(all ? "." : getFileName("testCleanup"));

		CPPUNIT_ASSERT( fileExists(testAll1.c_str()) == true );
		CPPUNIT_ASSERT( fileExists(testAll2.c_str()) == !all ); // tego nie powinno byæ je¿eli usuwamy wszystkie!

		for (tList::iterator it = keep.begin(); it != keep.end(); it++) {
			CPPUNIT_ASSERT( fileExists( it->strftime(format.c_str()).c_str() ) == true );
		}
		for (tList::iterator it = remove.begin(); it != remove.end(); it++) {
			CPPUNIT_ASSERT( fileExists( it->strftime(format.c_str()).c_str() ) == false );
		}

	}
	void testBackupCleanupDirectory() {
		testBackupCleanup(true);
	}
	void testBackupCleanupFile() {
		testBackupCleanup(false);
	}

	void testFileLockers() {
	}

	class Interface_test: public DT::Interface_passList {
	public:

		Interface_test() {
			passAsked = 0;
		}

		void showFileMessage(FileBase* file, const StringRef& _message, const StringRef& _title, bool error) {
			String msg;
			msg = error ? L"Wyst¹pi³ b³¹d w pliku " : L"Wyst¹pi³ problem z plikiem ";
			msg += "\"" + file->getFilename() + "\"\r\n\r\n";
			msg += _message; 

			String title = _title;
			title += " (" + Stamina::getFileName(file->getFilename()) + ")";

			std::cout << endl << title << endl  << msg << endl;
		}

		virtual Result handleFailedLoad(FileBase* file, DTException& e, int retry) {
			cout << "handleFailedLoad(" << file->getFilename() << ", " << e.getReason() << ", " << retry << ")" << endl;
			return __super::handleFailedLoad(file, e, retry);
		}
		virtual Result handleFailedSave(FileBase* file, DTException& e, int retry) {
			cout << "handleFailedSave(" << file->getFilename() << ", " << e.getReason() << ", " << retry << ")" << endl;
			return __super::handleFailedSave(file, e, retry);
		}
		virtual Result handleFailedAppend(FileBase* file, DTException& e, int retry) {
			cout << "handleFailedAppend(" << file->getFilename() << ", " << e.getReason() << ", " << retry << ")" << endl;
			return __super::handleFailedAppend(file, e, retry);
		}

		virtual Result handleRestoreBackup(FileBin* file, DTException& e, int retry) {
			cout << "handleRestoreBackup(" << file->getFilename() << ", " << e.getReason() << ", " << retry << ")" << endl;
			return __super::handleRestoreBackup(file, e, retry);
		}

		virtual MD5Digest askForPassword(FileBase* file, int retry) {
			passAsked ++;
			return DataTable::createPasswordDigest(password);
		}

		int passAsked;
		String password;

	};

	void testInterface() {
		createFile("testInterface");
		std::string filename = this->getFileName("testInterface");

		FileBin::backupFile(filename, false);

		FILE* f = fopen(filename.c_str(), "a");
		// dopisujemy bzdury zeby popsuc...
		fwrite(filename.c_str(), 1, filename.size(), f);
		fclose(f);

		DataTable dt;
		dt.setPassword(password);
		FileBin fb(dt);
		fb.makeBackups = true;
		
		enResult result;
		result = fb.loadAll(filename);
		CPPUNIT_ASSERT(result == DT::errBadFormat);

		Interface_test* iface = new Interface_test();
		oInterface oface = iface;
		iface->password = password;
		iface->disableRefCount();
		dt.setInterface(iface);
		dt.setPassword("z³e has³o");

		result = fb.loadAll(filename);
		CPPUNIT_ASSERT(result == DT::success);

		dt.setPassword("z³e has³o");

		result = fb.loadAll(filename);
		CPPUNIT_ASSERT(result == DT::success);

		CPPUNIT_ASSERT_EQUAL((int)1, iface->passAsked);

		dt.setPassword("z³e has³o");
		dt.setInterface(0);

		result = fb.append(filename);
		CPPUNIT_ASSERT(result == DT::errNotAuthenticated);

		dt.setInterface(iface);
		result = fb.append(filename);
		CPPUNIT_ASSERT(result == DT::success);
		
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
	CPPUNIT_TEST( testOldLoad );
	CPPUNIT_TEST( testChangeType );

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
	CPPUNIT_TEST( testOldLoad );
	CPPUNIT_TEST( testChangeType );


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
//CPPUNIT_TEST_SUITE_REGISTRATION( TestFileBinPass );
//CPPUNIT_TEST_SUITE_REGISTRATION( TestFileBinCryptAll );


