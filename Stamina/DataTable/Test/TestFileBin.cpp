#include <stdafx.h>
#include <cppunit/extensions/HelperMacros.h>
#include "..\DataTable.h"
#include "..\FileBin.h"
#include <Stamina\MD5.h>
#include <Stamina\Helpers.h>

using namespace Stamina;
using namespace Stamina::DT;

using std::string;

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

	TestFileBin() {
		FindFile ff(".\\TestFileBin__test*");
		deleteFiles( ff.makeList() );

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
		// jako drugi powinien zl�dowa� poprzedni trzeci...
		CPPUNIT_ASSERT_EQUAL( testString3, dt2.getStr(1, colString) );
		CPPUNIT_ASSERT_EQUAL( (unsigned int)2, dt2.getRowCount() );
		
	}

	void testBadPassword() {
		createFile("testPassword");

		DataTable dt;
		FileBin fb;
		dt.setPassword("z�ehas�o");
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
			std::cout << std::endl << "Zape�niam tablic� save" << std::endl;
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
			std::cout << std::endl << "Zape�niam tablic� append" << std::endl;
			cp.drawBar();
			std::cout << std::endl;
			for (; i < 2 * stressCount; i++) {
				tRowId row = dt2.addRow();
				dt2.setInt(row, colInt, i);
				dt2.setStr(row, colString, inttostr(i));
				cp.doStep();
			}
			ConsoleProgress cp2(0, appendCount);
			std::cout << std::endl << "Dopisuj� rekordy" << std::endl;
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
		{
			DataTable dt;
			dt.setXor1Key("\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84");
			FileBin fb(dt);
			CPPUNIT_ASSERT( fb.loadAll("TestFileBinOld.dtb") == success );
			CPPUNIT_ASSERT_EQUAL( (unsigned int) 3, dt.getRowCount() );

			CPPUNIT_ASSERT_EQUAL( string("Kontakt 0"), dt.getStr(0, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( string("Imi� 0"), dt.getStr(0, dt.getColumnId("Name")) );
			CPPUNIT_ASSERT_EQUAL( string("Kontakt 2"), dt.getStr(2, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( string("Imi� 2"), dt.getStr(2, dt.getColumnId("Name")) );

			FileBin fb2(dt);
			fb2.setFileFlag(FileBin::fflagCryptAll, cryptAll);
			if (!password.empty()) {
				dt.setPassword(password);
			}
			// powinno si� zapisa� tak samo...
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

		CPPUNIT_ASSERT_EQUAL( string("Kontakt 0"), dt3.getStr(0, dt3.getColumnId("Display")) );
		CPPUNIT_ASSERT_EQUAL( string("Imi� 0"), dt3.getStr(0, dt3.getColumnId("Name")) );
		CPPUNIT_ASSERT_EQUAL( string("Kontakt 2"), dt3.getStr(2, dt3.getColumnId("Display")) );
		CPPUNIT_ASSERT_EQUAL( string("Imi� 2"), dt3.getStr(2, dt3.getColumnId("Name")) );

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
		dt.setStr(1, dt.getColumnId("Display"), "Kontakt zmieniony");
		dt.setStr(1, dt.getColumnId("Name"), "Imi� zmienione");
		tRowId row = dt.insertRow(0);
		dt.setStr(row, dt.getColumnId("Display"), "Kontakt dodany");
		dt.setStr(row, dt.getColumnId("Name"), "Imi� dodane");
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

			CPPUNIT_ASSERT_EQUAL( string("Kontakt 0"), dt.getStr(0, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( string("Imi� 0"), dt.getStr(4, dt.getColumnId("Name")) );
			CPPUNIT_ASSERT_EQUAL( string("Kontakt 2"), dt.getStr(2, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( string("Imi� 2"), dt.getStr(6, dt.getColumnId("Name")) );
			CPPUNIT_ASSERT_EQUAL( string("Kontakt dodany"), dt.getStr(3, dt.getColumnId("Display")) );
			CPPUNIT_ASSERT_EQUAL( string("Imi� zmienione"), dt.getStr(5, dt.getColumnId("Name")) );
		}
	}

	void testChangeType() {
		createFile("testChangeType");

		DataTable dt;
		dt.setPassword(password);
		FileBin fb(dt);
		dt.setColumn(colString, ctypeInt, (DataEntry)testIntDef);
		fb.load(getFileName("testChangeType"));
		// powinien zignorowa� zawarto�� pliku i u�y� warto�ci domy�lnej...
		CPPUNIT_ASSERT_EQUAL( testIntDef, dt.getInt(0, colString) );
	}

	void testTemporary() {
		createFile("testTemporary");

		DataTable dt;
		dt.setPassword(password);
		FileBin fb(dt);
		fb.useTempFile = true;
		fb.loadAll(getFileName("testTemporary"));
		CPPUNIT_ASSERT_EQUAL(testString1, dt.getStr(row1, colString));
		// skoro jest za�adowane pr�bujemy nieudanego zapisu
		dt.setStr(row1, colString, testString2);
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
		CPPUNIT_ASSERT_EQUAL(testString1, dt.getStr(row1, colString));
		dt.setStr(row1, colString, testString2);
		fb.useTempFile = true;
		fb.save();
		fb.loadAll();
		CPPUNIT_ASSERT_EQUAL(testString2, dt.getStr(row1, colString));
	}

	/* 
	- czy .bak jest tworzony przy zapisie bez tempa
	- czy pomija tworzenie pliku .bak je� przed chwil� jeden zrobi�
	- czy .bak jest tworzony przy zapisie z temp'em
	- wyszukiwanie ostatniego .bak'a
	- przywracanie .bak'�w
	*/
	void testBackup() {
		createFile("testBackup");
		std::string filename = this->getFileName("testBackup");

		DataTable dt;
		dt.setPassword(password);
		FileBin fb(dt);
		fb.loadAll(filename);
		dt.setStr(row1, colString, testString2);
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
		// nie powinien zrobi� backupu
		fb.backupFile(false);
		CPPUNIT_ASSERT(fb.findLastBackupDate().empty() == true);
		dt.setTimeLastBackup(Time64());
		// powinien zrobi� backup
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
		CopyFile(filename.c_str(), (filename + Time64(Time64(true) - 120).strftime(".%d-%m-%Y %H-%M-%S.bak")).c_str(), false);
		CPPUNIT_ASSERT_EQUAL(backup, fb.findLastBackupFile());

		// przywracanie
		createFile("testBackup");
		fb.restoreLastBackup();
		fb.loadAll();
		CPPUNIT_ASSERT_EQUAL(testString2, dt.getStr(row1, colString));

	}

	/*
	- czy usuwa prawid�owe pliki
	- czy dzia�a prawid�owo tryb plik/katalog
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

		std::string format = getFileName("testCleanup") + ".%d-%m-%Y %H-%M-%S.bak";

		for (tList::iterator it = keep.begin(); it != keep.end(); it++) {
			CopyFile("TestFileBinOld.dtb", it->strftime(format.c_str()).c_str() , false);
		}
		for (tList::iterator it = remove.begin(); it != remove.end(); it++) {
			CopyFile("TestFileBinOld.dtb", it->strftime(format.c_str()).c_str() , false);
		}

		std::string testAll1 = getFileName("testCleanupAll") + ".10-06-2001 12-00-00.bak";
		std::string testAll2 = getFileName("testCleanupAll") + ".09-06-2001 12-00-00.bak";
		CopyFile("TestFileBinOld.dtb", testAll1.c_str(), false);
		CopyFile("TestFileBinOld.dtb", testAll2.c_str(), false);

		FileBin::cleanupBackups(all ? "." : getFileName("testCleanup"));

		CPPUNIT_ASSERT( fileExists(testAll1.c_str()) == true );
		CPPUNIT_ASSERT( fileExists(testAll2.c_str()) == !all ); // tego nie powinno by� je�eli usuwamy wszystkie!

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


