/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

//#include <mem.h>
#include "stdafx.h"
#include "FileBin.h"
#include "Crypt.h"



#include <Stamina\Assert.h>
#include <Stamina\FindFileFiltered.h>
#include <Stamina\WideChar.h>

using namespace std;

namespace Stamina { namespace DT {

	FileBin::FileBin() { 
		init(); 
	}
	
	FileBin::FileBin(DataTable& t) {
		init();
		assign(t);
	}

	FileBin::~FileBin() {
		this->close();
	}

	void FileBin::init() {
		_passwordSalt = 0;
		_xorSalt = 0;

		_fcols.setLoader(true);
		_pos_data = _pos_rows = _pos_cols = 0;
		_fileFlag = fflagNone;
		_dataSize = 0;
		_dataFlag = basicDataFlags;
		useTempFile = false;
		makeBackups = false;
		warningDialogs = false;
		//warn = true;
//		mode = 0;
    }

	void FileBin::reset() {
		FileBase::reset();
		_passwordSalt = 0;
		_xorSalt = 0;
		_passwordDigest.reset();
		_xorDigest.reset();
		_dataFlag = basicDataFlags;
	}

	/**Checks table's password digest with the file's one*/
	bool FileBin::isAuthenticated() {
		if (!_table) return false;
		MD5Digest digest(_table->getPasswordDigest());
		if (_passwordDigest.empty()) return true;
		if (digest.empty()) {
			digest.calculate("");
		}
		if (_passwordSalt)
			digest.addSalt(_passwordSalt);
		return digest == _passwordDigest;
	}


	void FileBin::generatePasswordDigest(bool newSalt) {
		_passwordDigest = _table->getPasswordDigest();
		if (_passwordDigest.empty()) {
			_passwordDigest = MD5Digest("");
			_passwordSalt = 0;
		}
		if (newSalt || !_passwordSalt)
			_passwordSalt = random(1, 0x7FFFFFFF);
		_passwordDigest.addSalt(_passwordSalt);
	}

	void FileBin::generateXorDigest(bool newSalt) {
		_xorDigest = _table->getPasswordDigest();
		if (_xorDigest.empty()) {
			_xorDigest = MD5Digest("");
		}
		if (newSalt || !_xorSalt)
			_xorSalt = random(1, 0x7FFFFFFF);
		_xorDigest.addSalt(_xorSalt);
	}

	void FileBin::open (const std::string& fileToOpen , enFileMode mode) {
		this->close();

		if (!fileToOpen.empty())
			_fileName = fileToOpen;
		if (!_table) throw DTException( errNotInitialized );
		if (_fileName.empty())
			throw DTException( errFileNotFound );

		this->setWriteFailed( false );
		_storedRowsCount=0;

		this->_opened = fileClosed;
		if (mode & fileRead) {
			_table->_lastId = rowIdMin;
	        //_table->_db = -1;
		}

		_pos_state = _pos_cols = _pos_data = _pos_dataLastId = _pos_rows = 0;
		this->_headerLoaded = false;

		_fileName = getFullPathName(_fileName.c_str());
	    
		if (useTempFile && (mode & fileWrite) && !(mode & fileRead)) {
			_temp_fileName = _fileName;
			int i = 0;
			do {
				_temp_fileName = _fileName + stringf(".[%d].tmp", ++i);
			} while ( ! _access(_temp_fileName , 0));
		} else {
			_temp_fileName = "";
		}

		bool fileExists = (_access(this->getOpenedFileName().c_str(),0) == 0);

		const char* openmode;
		// Dla Append, oraz Read+Write otwieramy w trybie read+write i jeœli jest taka potrzeba, tworzymy plik na nowo
		if (((mode & fileAppend) || (mode & fileWrite && mode & fileRead))) {
			openmode = fileExists ? "r+b" : "w+b";
			_recreating = (fileExists == false);
			_table->_timeModified.now();
		} else if (mode & fileWrite) {
			openmode = "wb";
			_recreating = true;
			_table->_timeModified.now();
			if (this->makeBackups && useTempFile == false && _temp_fileName.empty()) {
				// je¿eli nie u¿ywamy tempa backup musi byæ ju¿ teraz!
				this->backupFile(true);
			}
		} else {
			openmode = "rb";
			_recreating = false;
		}

		if (_table->_timeCreated.empty()) 
			_table->_timeCreated.now();


		_file=fopen(this->getOpenedFileName().c_str(), openmode);

		if (_file && useTempFile && (mode & fileWrite)) {
			_temp_enabled = true;
		} else {
			_temp_enabled = false;
			_temp_fileName = "";
		}

		if (!_file) throw DTFileException();

		this->updateFileSize();

		this->_opened = mode;

		// Zawsze wczytujemy nag³ówek, chyba ¿e bêdziemy tworzyæ plik od nowa.
		if (isReadable()) {
			this->readHeader();
		} else {
			// W przeciwnym razie uznajemy ¿e to co posiadamy jest "za³adowanym" nag³ówkiem
			this->_headerLoaded = true;
			this->_verMaj = DT::binVersionMaj;
			this->_verMin = DT::binVersionMin;
		}

    }

    void FileBin::close () {
		if (!this->isOpened()) return;

		this->writeState();
        fclose(_file);
		
		if (this->isWriteFailed()) {
		    _opened = fileClosed;
			
#ifdef _WINDOWS_
			if (this->warningDialogs) {
				std::string msg = stringf("Wyst¹pi³ b³¹d podczas zapisywania danych!\r\nUszkodzona kopia znajduje siê w:\r\n\t%s%s" , _temp_enabled ? _temp_fileName.c_str() : _fileName.c_str(), _temp_enabled ? "\r\n\r\nOryginalny plik pozosta³ bez zmian..." : "\r\n\r\nB³¹d wyst¹pi³ podczas zapisywania oryginalnej kopii!");
				int r = MessageBox(0 , msg.c_str() , "B³¹d zapisu .DTB" , MB_OK | MB_ICONERROR | MB_TASKMODAL);
			}
#endif
			throw DTException(errWriteError);
			return;
		}

        if (_temp_enabled) { // trzeba przerzuciæ tempa
            bool success = true;
			int retries = 0;

			if (this->makeBackups) {
				// je¿eli u¿ywamy tempa backup musi byæ teraz!
				this->backupFile(true);
			}


            while (1) {
				_unlink(_fileName.c_str());
				if (MoveFileEx(_temp_fileName.c_str() , _fileName.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == TRUE)
					break; // uda³o siê!

				if (++retries < 8) { // czekamy 2 sekundy
					Sleep(250);
					continue;
				}
#ifdef _WINDOWS_
				std::string msg = stringf("Nie mogê zapisaæ danych do pliku\r\n   %s" , _fileName.c_str());
				int r = MessageBox(0 , msg.c_str() , "B³¹d zapisu .DTB" , MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON2 | MB_TASKMODAL);
                if (r == IDIGNORE) {
					success=true;
					break;
				} else if (r == IDABORT) {
	                success = false;
					break;
				}
#endif
            }
            if (!success) _unlink(_temp_fileName);
		}

		_opened = fileClosed;
    }


// HEADER ** READ -----------------------------------------------------------

	void FileBin::readHeader() {
		if (!isOpened()) throw DTException(errNotOpened);
		if (this->_headerLoaded == true) return;

		this->seekToBeginning();
      
		char sig [6];
		sig[5]='\0';
		readData(sig , 5);
		if (strcmp("DTBIN" , sig)) throw DTException(errBadFormat);

		readData(&_verMaj , 1);
		if (_verMaj > DT::binVersionMaj) throw DTException(errBadVersion);

		// £adujemy drugi cz³on wersji, oraz flagi (dostêpne od v2.0)
	    if (_verMaj > '1') {
			readData(&_verMin , 1);
			_pos_state = ftell(_file);
			readData(&_fileFlag , 4);
		} else {
			_pos_state = ftell(_file);
			_verMin = 0; 
			_fileFlag = fflagNone;
		}

		// £adujemy liczbê wierszy
		readData(&_storedRowsCount, 4);  // rowc
		_table->_size = _storedRowsCount;
      
		_dataFlag = dflagNone;
        unsigned int dataLeft = 0;
		// Pole DATA (od v2.0)
		if (_verMaj > '1') {
			readData(&_dataSize , 4);
	        _pos_data = ftell(_file);
			dataLeft = _dataSize;
		} else {
			_dataSize = 0;
		}
        if (_dataSize) {
			readData(&_dataFlag , 4, &dataLeft);
		}

        // £adujemy wszystkie wyszczególnione pola
		if (_dataFlag & dflagLastId) {
			_pos_dataLastId = ftell(_file);
			readData(&_table->_lastId , 4, &dataLeft);
		} else {
			_pos_dataLastId = 0;
			_table->_lastId = rowIdMin;
		}
        
		if (_dataFlag & dflagPasswordDigest) {
			MD5Digest::tDigest digest;
			readData(digest , 16, &dataLeft);
			this->_passwordDigest.setDigest(digest);
		} else {
			this->_passwordDigest.reset();
		}

/*		if (_dataFlag & dflagDBId) {
			readData(&_table->_dbID , 4, &dataLeft);
		} else {
			_table->_dbID = -1;
		}*/
		if (_dataFlag & dflagDBId) { // nie obs³ugujemy ju¿ tego!
			this->setFilePosition(4, fromCurrent);
			dataLeft -= 4;
		}


		if (_dataFlag & dflagParams) {
			int paramCount;
			readData(&paramCount, 4, &dataLeft);
			for (int i = 0; i < paramCount; i++) {
				String key = readString(&dataLeft);
				String value = readString(&dataLeft);
				if (_table->paramExists(key) == false) {
					_table->setParam(key, value);
				}
			}
		}

		if (_dataFlag & dflagCreated) {
			readData(&_table->_timeCreated, 8, &dataLeft);
		} else {
			_table->_timeCreated.now();
		}

		if (_dataFlag & dflagModified) {
			readData(&_table->_timeModified, 8, &dataLeft);
		} else {
			_table->_timeModified.now();
		}

		if (_dataFlag & dflagLastBackup) {
			readData(&_table->_timeLastBackup, 8, &dataLeft);
		} else {
			_table->_timeCreated.clear();
		}

		if (_dataFlag & dflagPassSalt) {
			readData(&_passwordSalt, 4, &dataLeft);
		} else {
			_passwordSalt = 0;
		}

		if (_dataFlag & dflagXorSalt) {
			readData(&_xorSalt, 4, &dataLeft);
		} else {
			_xorSalt = 0;
		}


		// pomijamy ca³¹ resztê której nie rozumiemy...
		if (dataLeft > 0) {
			this->setFilePosition(dataLeft, fromCurrent);
		}

		_pos_cols = ftell(_file);

		this->_headerLoaded = true;
		this->authenticate();

		// je¿eli uda³o nam siê zautentykowaæ, znaczy ¿e has³o jest prawid³owe i mo¿emy spokojnie tworzyæ maskê xor
		this->generateXorDigest(false);
	}

// HEADER ** WRITE ----------------------------------------------------------

	void FileBin::writeHeader() {
		if (!isOpened()) throw DTException(errNotOpened);

		try {

			// generujemy nowe has³o i maskê xor z nowymi saltami
			this->generatePasswordDigest(true);
			this->generateXorDigest(true);

			writeData("DTBIN", 5); 

			writeData(&_verMaj, 1);
			writeData(&_verMin, 1);

			_pos_state = ftell(_file);

			if (this->versionNewCrypt() == false) {
				_fileFlag = (enFileFlags)(_fileFlag & ~fflagCryptAll);
			}
			writeData(&_fileFlag, 4); // flag

			unsigned int rowCount = _table->getRowCount();
			writeData(&rowCount, 4);    // rowc

			_dataSize = 0x7FFFFFFF;
			// placeholder do zapisania trochê póŸniej. Specjalnie zapisujemy du¿¹ wartoœæ, ¿eby w razie pozostawienia tego w takim stanie w pliku spowodowaæ "bezpieczny" b³¹d podczas wczytywania
			writeData(&_dataSize, 4);
			_dataSize = 0;

			_pos_data = ftell(_file);

			// usuwamy nie obs³ugiwane flagi
			_dataFlag = (enDataFlags)(_dataFlag & ~(deprecatedDataFlags));
			// dodajemy wymagane obs³ugiwane flagi
			_dataFlag = (enDataFlags)(_dataFlag | requiredDataFlags);

			if (!this->versionNewCrypt()) {
				_dataFlag = (enDataFlags)(_dataFlag & ~(newDataFlags));
			}

			if (! _table->getParamsMap().empty()) {
				_dataFlag = (enDataFlags)(_dataFlag | dflagParams);
			}

			if (_dataFlag) {
				writeData(&_dataFlag, 4, &_dataSize);
			}

			if (_dataFlag & dflagLastId) {
				_pos_dataLastId = ftell(_file);
				writeData(&_table->_lastId, 4, &_dataSize);
			} else {
				_pos_dataLastId = 0;
			}

			if (_dataFlag & dflagPasswordDigest) {
				if (versionNewCrypt()) {
					writeData(_passwordDigest.getDigest(), 16, &_dataSize);
				} else {
					writeData(_table->getPasswordDigest().getDigest(), 16, &_dataSize);
				}
			}

			if (_dataFlag & dflagParams) {
				unsigned int paramsCount = _table->getParamsMap().size();
				writeData(&paramsCount, 4, &_dataSize);
				for (DataTable::tParams::const_iterator it = _table->getParamsMap().begin(); it != _table->getParamsMap().end(); it++) {
					writeString(it->first, &_dataSize);				
					writeString(it->second, &_dataSize);				
				}
			}
			if (_dataFlag & dflagCreated) {
				writeData(&_table->_timeCreated, 8, &_dataSize);
			}

			if (_dataFlag & dflagModified) {
				writeData(&_table->_timeModified, 8, &_dataSize);
			}

			if (_dataFlag & dflagLastBackup) {
				writeData(&_table->_timeLastBackup, 8, &_dataSize);
			}

			if (_dataFlag & dflagPassSalt) {
				writeData(&_passwordSalt, 4, &_dataSize);
			}

			if (_dataFlag & dflagXorSalt) {
				writeData(&_xorSalt, 4, &_dataSize);
			}

			// Zapisujemy w³aœciwy rozmiar...
			this->setFilePosition(_pos_data - 4, fromBeginning);
			writeData(&_dataSize, 4);
			// Ustawiamy siê z powrotem za danymi
			this->setFilePosition(_dataSize, fromCurrent);

			fflush(_file);

			this->updateFileSize();

		} catch (DTException e) {
			this->setWriteFailed(true);
			throw e;
		}
	}

// DESCRIPTOR ** READ -------------------------------------------------------

	void FileBin::readDescriptor() {
		if (!isOpened()) throw DTException(errNotOpened);

		_fcols.clear();

		this->setFilePosition(_pos_cols, fromBeginning);

		int count;
		readData(&count, 4);
		_fcols.setColumnCount(count);
		for (int i = 0; i < count; i++) { // columns definitions
			tColId id;
			enColumnType type;
			readData(&id, 4);
			readData(&type, 4);
			std::string name;
			/*Wczytujemy nazwê kolumny i dane dodatkowe (od v3.0)*/
			if (_verMaj>'2') {
				unsigned char length;
				readData(&length, 1);
				if (length) {
					readData(stringBuffer(name, length), length);
					stringRelease(name, length);
				}
				int dataSize;
				readData(&dataSize , 4);
				if (dataSize) {
					setFilePosition(dataSize, fromCurrent); // We have to get past unprocessed data.
				}
			}
			_fcols.setColumn(id, type, name.c_str());
		}
		_pos_rows = ftell(_file);
	}

// DESCRIPTOR ** WRITE-------------------------------------------------------

	void FileBin::writeDescriptor() {
		if (!isOpened()) throw DTException(errNotOpened);

		try {

			unsigned int colCount = _fcols.getColCount();
			writeData(&colCount, 4);

			for (unsigned int colIndex = 0; colIndex < colCount; colIndex++) {
				const Column& col = *_fcols.getColumnByIndex(colIndex);
				tColId id = col.getId();
				writeData(&id, 4);   //id
				int type = col.getFlags(); 
				writeData(&type, 4);   //type
				unsigned char nameLength = min(255, col.getName().size());
				writeData(&nameLength , 1); // name length
				if (nameLength) 
					writeData(col.getName().c_str(), nameLength); // name
				unsigned int dataSize = 0;
				writeData(&dataSize, 4); // For future use maybe.
			}
	      
			_pos_rows = ftell(_file);

			this->updateFileSize();

		} catch (DTException e) {
			this->setWriteFailed(true);
			throw e;
		}
	}

// SIZE ** READ -------------------------------------------------------------
/*
	void FileBin::readCount() {
		if (!isOpened()) throw DTException(errNotOpened);
		// Ustawiamy siê na pozycjê zawieraj¹c¹ rozmiar
		setFilePosition(_pos_count, fromBeginning);
		readData(&_storedRowsCount, 4);
    }
*/

// SIZE ** WRITE ------------------------------------------------------------

	void FileBin::writeState() {
		if (!isOpened()) throw DTException(errNotOpened);
		try {
			if ((getFileMode() & (fileWrite | fileAppend))) {
				S_ASSERT(_pos_state);
                setFilePosition(_pos_state , fromBeginning);
				if (_verMaj > '1') {
					writeData(&_fileFlag, 4);
				}
				if (_storedRowsCount != -1) {
					writeData(&_storedRowsCount, 4);
				} else {
					setFilePosition(4, fromCurrent);
				}
				// zapisujemy lastId (od v2.0)
				if (_table && _verMaj > '1' && _pos_dataLastId > 0) {
					setFilePosition(_pos_dataLastId, fromBeginning);
					writeData(&_table->_lastId, 4);
				}
			}
		} catch (DTException e) {
			this->setWriteFailed(true);
			throw e;
		}
    }

// ROW ** READ --------------------------------------------------------------

	enResult FileBin::readPartialRow(tRowId row , tColId* columns, bool readId) {
		if (!isOpened()) throw DTException(errNotOpened);

		// Pokojowe wyjœcie - nie ma co czytaæ wiêc koñczymy
		if ( feof(_file) ) return resNothingToRead;
		// Od tego momentu wszystko czego nie da siê wczytaæ oznacza z³y format!

		row = _table->getRowPos(row);
	  
		//_table->notypecheck=1;  // wylacza sprawdzanie typow ...
      
		oDataRow rowObj = _table->getRow(row);
		
		// zapiujemy pozycjê wiersza
		rowObj->_filePos = ftell(_file);

		// Znacznik rozpoczêcia nowego wiersza
		if (fgetc(_file) != '\n') {
			if ( feof(_file) ) return resNothingToRead;
			throw DTException(errBadFormat);
		}

		unsigned int rowSize = 0;
		unsigned int rowDataSize = 0;
		enRowDataFlags rowDataFlag;
		unsigned int rowDataLeft;

		// rozmiar wiersza oraz flagi od v2.0
		if (_verMaj > '1') {
			readData(&rowSize, 4);
			// Sprawdzamy czy ten row jest w stanie siê tu w ogóle zmieœciæ
			if (ftell(_file) + rowSize + 4 > _fileSize)
				throw DTException(errBadFormat);

			//TODO: mo¿e to wy³¹czyæ??
			// Skaczemy do przodu ¿eby sprawdziæ czy rozmiary siê zgadzaj¹
			setFilePosition(rowSize , fromCurrent);
			unsigned int rowSize2;
			readData(&rowSize2 , 4);
			setFilePosition(- (signed int)rowSize - 4, fromCurrent); // Wracamy do pozycji
			if (rowSize != rowSize2) 
				throw DTException(errBadFormat);
            
			readData(&rowObj->_flag, 4);

			// Sprawdzamy czy flaga jest równa -1 czyli czy element
			// nie jest oznaczony jako usuniêty
			if (rowObj->_flag == -1) {
				setFilePosition(rowSize, fromCurrent);
				// wywo³ywanie rekurencyjne jest potencjalnie niebezpieczne...
				return resSkipped;
			}

			readData(&rowDataSize, 4);
			if (rowDataSize > rowSize - 8)
				throw DTException(errBadFormat);

			rowDataLeft = rowDataSize;

			if (rowDataSize) {
				readData(&rowDataFlag, 4, &rowDataLeft);
			} else {
				rowDataFlag = rdflagNone;
			}
			
		} else { // v >= 2.0
			rowDataFlag = rdflagNone;
			rowDataSize = 0;
			rowDataLeft = 0;
		}


		if (rowDataFlag & rdflagRowId) {
			tRowId id;
			readData(&id, 4, &rowDataLeft);
			id = DataTable::flagId(id);
			if (readId && id != rowObj->getId()) {
				if (_table->rowIdExists(id)) {
					id = _table->getNewRowId();
				}
				rowObj->setId(id);
			}
		} else {
			// w zasadzie nie ma potrzeby przydzielaæ nowego ID, bo jest ju¿ przydzielony przy okazji utworzenia wiersza... Poza tym ka¿dy szanuj¹cy siê DTB zawiera t¹ wartoœæ...
			// TODO: sprawdziæ!
		}

		// Pomijamy dane których nie rozumiemy...
		if (rowDataLeft > 0) setFilePosition(rowDataLeft, fromCurrent);

		// £adujemy dane
		for (unsigned int colIndex = 0; colIndex < _fcols.getColCount(); colIndex++) {
			const Column& col = *_fcols.getColumnByIndex(colIndex);

			// skoro kolumna nie s³u¿y do zapisywania - nie mamy co wczytywaæ...
			if (col.hasFlag(cflagDontSave)) continue;

			// identyfikator kolumny w TABLICY
			tColId colId = col.getId();

			if (col.isIdUnique()) { 
				colId = _table->getColumns().getNameId(col.getName().c_str()); 
			}

			bool skip = (colId == colNotFound); // Czy OMIN¥Æ dane kolumny?

			const Column& tableCol = *_table->getColumns().getColumn(colId);
			if (!skip && (tableCol.hasFlag(cflagDontSave) || tableCol.getType() != col.getType())) {
				skip = true;
			}

			// szukamy, czy ID aktualnej, jest na liœcie, jak nie to omijamy...
			if (!skip && columns) { 
				skip = true;
				int i = 0;
				do {
					if (colId == columns[i]) {
						skip = false;
						break;
					}
					i++;
				} while (columns[i]);
			}

			unsigned int skipBytes = 0;

			switch (col.getType()) {
				case ctypeInt:
					if (skip)
						skipBytes = 4;
					else {
						int val;
						readCryptedData(&col, &val, 4);
						tableCol.setInt( rowObj, val );
					}
					break;
				case ctypeInt64:
					if (skip) {
						skipBytes = 8;
					} else {
						__int64 val;
						readCryptedData(&col, &val, 8);
						tableCol.setInt64( rowObj, val );
					}
					break;
				case ctypeDouble:
					if (skip) {
						skipBytes = 8;
					} else {
						double val;
						readCryptedData(&col, &val, 8);
						tableCol.setDouble( rowObj, val );
					}
					break;
				case ctypeString: {
					if (skip) {
						readData(&skipBytes, 4);
					} else {
						tableCol.setString( rowObj, this->readString(&col) );
					}

					break;}

				case ctypeBin: {
					int size;
					readData(&size, 4); // wczytujemy rozmiar
					if (ftell(_file) + size > _fileSize)
						throw DTException(errBadFormat);
					if (skip) {
						skipBytes = size;
					} else if (size > 0) {
						ByteBuffer buff(size);
						readCryptedData(&col, buff.getBuffer(), size);
						buff.setLength(size);
						tableCol.setBin( rowObj, buff );
					} else {
						ByteBuffer buff;
						tableCol.setBin( rowObj, buff );
					}
					break; }

			}
			if (skipBytes > 0) {
				setFilePosition(skipBytes, fromCurrent);
			}
			if (fgetc(_file) != '\t')
				throw DTException(errBadFormat);
		} // kolumny

		// drugi rozmiar od v2.0
		if (_verMaj > '1') {
			unsigned int rowSize2;
			readData(&rowSize2, 4);
			if (rowSize != rowSize2) 
				throw DTException(errBadFormat);
		}
		return success;
    }


// ROW ** WRITE -------------------------------------------------------------

	void FileBin::writeRow(tRowId row) {
		if (!isOpened()) throw DTException(errNotOpened);
		try {

			row = _table->getRowPos(row);

			if (fputc('\n' , _file) == EOF)
				throw DTFileException();

			oDataRow rowObj = _table->getRow(row);

			unsigned int rowSize = 0;

			// rozmiar, flagi - od v2.0
			if (_verMaj > '1') {
				rowSize = 0x7FFFFFFF;
				writeData(&rowSize, 4);  //rowSize - placeholder
				rowSize = 0;
				enRowFlag rowFlags = rowObj->getFlags();
				writeData(&rowFlags, 4, &rowSize); // flag
				unsigned int dataSize = 8; // flag + lastId, na razie nie ma wiêcej
				writeData(&dataSize, 4, &rowSize);
				enRowDataFlags flags = rdflagRowId;
				writeData(&flags, 4, &rowSize);
				tRowId rowId = rowObj->getId();
				rowId = DataTable::unflagId(rowId);
				writeData(&rowId, 4, &rowSize);
			}
			for (unsigned int colIndex =0; colIndex < _fcols.getColCount(); colIndex++) {
				const Column& col = *_fcols.getColumnByIndex(colIndex);

				if (col.hasFlag(cflagDontSave)) continue;

				tColId colId = col.getId();
				if (col.isIdUnique()) { 
					colId = _table->getColumns().getNameId(col.getName().c_str()); 
				}
				const Column& tableCol = *_table->getColumns().getColumn(colId);
				// nie konwertujemy danych przy zapisywaniu, po prostu wstawiamy puste wartoœci...
				bool skip = (tableCol.getType() != col.getType());

				switch (col.getType()) {
					case ctypeInt: {
						int val = skip ? 0 : tableCol.getInt(rowObj);
						writeCryptedData(&col, &val, 4, &rowSize);
						break;}
					case ctype64: {
						__int64 val = skip ? 0 : tableCol.getInt64(rowObj);
						writeCryptedData(&col, &val, 8, &rowSize);
						break;}
					case ctypeString: {
						StringRef val;
						if (!skip) {
							val = PassStringRef( tableCol.getString(rowObj, false) );
						}
						this->writeString(val, &col, &rowSize);
						break;}
					case ctypeBin: {
						ByteBuffer buff;
						if (!skip) {
							buff.assignCheapReference( tableCol.getBin(rowObj, false) );
						}
						unsigned int size = buff.getBufferSize();
						writeData(&size, 4, &rowSize);
						if (size > 0) {
							writeCryptedData(&col, buff.getBuffer(), size, &rowSize);
						}
						break;}
				}

				if (fputc('\t' , _file) == EOF)
					throw DTFileException();
				rowSize++;
			}
			// zapisujemy wynik od v2.0
			if (_verMaj > '1') {
				writeData(&rowSize, 4); // size
				// cofamy siê o ca³y wiersz i oba zapisane size'y
				setFilePosition(- (signed int)rowSize - 8, fromCurrent);
				writeData(&rowSize, 4);
				// idziemy do przodu o ca³y wiersz i ostatni size...
				setFilePosition(rowSize + 4, fromCurrent);
			}
			_storedRowsCount++;
		} catch (DTException e) {
			this->setWriteFailed(true);
			throw e;
		}
	}


// ----------------------------------------------------------------------




	void FileBin::setErasedRow(bool overwrite , int testIndex) {
		if (!isOpened()) throw DTException(errNotOpened);
        if (_verMaj && _verMaj < '3') 
			throw DTException(errBadVersion);
        if (feof(_file)) throw DTException(errRowNotFound);
		try {
			if (fgetc(_file) != '\n') 
				throw DTException(errBadFormat);
			if (feof(_file)) 
				throw DTException(errBadFormat);

			int rowSize, rowSize2;
			long pos;
			// Najpierw sprawdza czy zgadza siê rozmiar...
			readData(&rowSize, 4);
			pos = ftell(_file);
			setFilePosition(rowSize, fromCurrent);
			readData(&rowSize2, 4);
			if (rowSize != rowSize2) throw DTException(errBadFormat); // Oba rozmiary nie s¹ zgodne!!!
			// Sprawdzamy index jeœli jest taka potrzeba
			if (testIndex) {
				setFilePosition(pos + 4, fromBeginning); // Wracamy do pocz¹tku i pomijamy flagi
				int rowDataSize = 0;
				readData(&rowDataSize, 4);
				if (rowDataSize >= 8) { // Potrzebujemy równie¿ flagê...
					int rowDataFlag = 0;
					readData(&rowDataFlag, 4);
					if (rowDataFlag & rdflagRowId) {
						int id = 0;
						readData(&id, 4);
						if (id != DataTable::unflagId(testIndex)) throw DTException(errRowNotFound); // Z³y index!
					}
				}
			}
			if (this->_storedRowsCount != -1 && this->_storedRowsCount) { 
				// A mo¿e ju¿ jest ustawione?
				setFilePosition(pos, fromBeginning); // Wracamy do pozycji
				int oldFlag = 0;
				readData(&oldFlag, 4);
				if (oldFlag == -1) return;
				this->_storedRowsCount --;
			} else this->_storedRowsCount = -1;

			setFilePosition(pos, fromBeginning); // Wracamy do pozycji
			int newFlag = -1;
			writeData(&newFlag, 4); // Zapisujemy flagê

			if (overwrite) {
				// Zerujemy zawartoœæ 
				char buff [100];
				memset(buff , 0 , 100);
				size_t size = rowSize - 4;
				size_t i = 0;
				while (i < size) {
					if (!fwrite(buff, min(100 , size - i), 1, _file)) break;
					i += min(100 , size - i);
				}
			}
			// Zaznaczamy flage (zostanie zapisana do pliku w close()
			this->setFileFlag(fflagFragmented, true);
			// Skaczemy do nastepnego wiersza
			setFilePosition(pos + rowSize + 4, fromBeginning); 
		} catch (DTException e) {
			this->setWriteFailed(true);
			throw e;
		}
    }

	bool FileBin::findNextRow() {
		if (_verMaj <= '1' || feof(_file))
			return false;
		while (!feof(_file)) {
			// szukamy '\n'
			if (getc(_file) == '\n') {
				setFilePosition(-1, fromCurrent);
				return true; // znalaz³em potencjalny nowy wpis... freadrow zajmie siê sprawdzaniem...
			}
		}
		return false;
	}


	void FileBin::readRows(bool skipFailed) {
        _table->clearRows();
        tRowId row;
        while (!feof(_file))
        { 
			row = _table->addRow();
			if (skipFailed == false) {
				if (this->readRow(row) != success)
					_table->deleteRow(row);
			} else {
				while (1) {
					try {
						if (this->readRow(row) != success)
							_table->deleteRow(row);
					} catch (DTException e) {
						if (findNextRow() == true) {
							continue; // jeszcze raz readRow
						} else {
							_table->deleteRow(row);
						}
					}
					// wyskakujemy z pêtli ¿eby wczytaæ nastêpny
					break;
				}
			}
		
		}
	}

	void FileBin::readCryptedData(const Column* col, void* buffer, int size, unsigned int* decrement) {
		if ((col && col->hasFlag(cflagXor)) || this->hasFileFlag(fflagCryptAll)) {
			if (this->versionNewCrypt()) {
				size_t salt = ftell(_file);
				this->readData(buffer, size, decrement);
				xor2_decrypt(_xorDigest.getDigest(), (unsigned char*)buffer, size, salt);
			} else {
				this->readData(buffer, size, decrement);
				xor1_decrypt(_table->getXor1Key(), (unsigned char*)buffer, size);			}
		} else {
			this->readData(buffer, size, decrement);
		}

	}

	void FileBin::writeCryptedData(const Column* col, const void* buffer, int size, unsigned int* increment) {
		if ((col && col->hasFlag(cflagXor)) || this->hasFileFlag(fflagCryptAll)) {
			unsigned char* crypted = new unsigned char [size];
			memcpy(crypted, buffer, size);
			if (this->versionNewCrypt()) {
				xor2_encrypt(_xorDigest.getDigest(), crypted, size, ftell(_file));
			} else {
				xor1_encrypt(_table->getXor1Key(), crypted, size);
			}
			this->writeData(crypted, size, increment);
			delete [] crypted;
		} else {
			this->writeData(buffer, size, increment);
		}
	}


	inline PassStringRef FileBin::readString(const Column* col, unsigned int* decrement) {

		unsigned int length;
		readData(&length, 4, decrement);
		if (ftell(_file) + length > _fileSize)
			throw DTException(errBadFormat);

		String val;

		if (length > 0) {
			int codePage = GetACP();
			if (this->versionNewString()) {
				readData(&codePage, 4, decrement);
				length -= 4;
			}

			if (codePage == -1) { //Unicode
				readCryptedData(col, val.useBuffer<wchar_t>(length), length, decrement);
				val.releaseBuffer<wchar_t>(length);
			} else {
				char * buffer = val.useBuffer<char>(length);
				readCryptedData(col, buffer, length, decrement);
				val.releaseBuffer<char>(length);
				if (codePage == GetACP()) {
					val.assignCheapReference( buffer, length);
				} else {
					val.assign( toUnicode(buffer, codePage) );
				}
			}
		}
		return val;
	}

	inline void FileBin::writeString(const StringRef& val, const Column* col, unsigned int* increment)  {
		if (versionNewString()) {
			int codepage;
			unsigned int length;
			const void* buffer;
			if (val.isWide()) {
				length = val.getDataSize<wchar_t>() * 2;
				codepage = -1;
				buffer = val.getData<wchar_t>();
			} else {
				length = val.getDataSize<char>();
				codepage = GetACP();
				buffer = val.getData<char>();
			}
			if (length > 0) length += 4;
			writeData(&length, 4, increment);
			if (length > 0) {
				writeData(&codepage, 4, increment);
				writeCryptedData(col, buffer, length, increment);
			}
		} else {
			unsigned int length = val.getDataSize<char>();
			writeData(&length, 4, increment);
			if (length > 0) {
				writeCryptedData(col, val.getData<char>(), length, increment);
			}
		}

	}



// BACKUP ----------------------------------------------------------

	Date64 getBackupDate(const RegEx& re) {
		Date64 d;
		d.day = atoi(re[2].c_str());
		d.month = atoi(re[3].c_str());
		d.year = atoi(re[4].c_str());
		d.hour = atoi(re[5].c_str());
		d.min = atoi(re[6].c_str());
		d.sec = atoi(re[7].c_str());
		return d;
	}

	FindFile::Found findLastBackup(const std::string& filename, Date64* time = 0) {
		FindFileFiltered ff(getFileDirectory(filename, true) + "\\*.bak");
		ff.setFileOnly();
		FileFilter_RegEx& re = *(new FileFilter_RegEx("/^(" + RegEx::addSlashes(getFileName(filename)) + ")\\.(\\d+)-(\\d+)-(\\d+) (\\d+)-(\\d+)-(\\d+).bak$/i"));
		ff.addFilter(re);
		FindFile::Found found;
		Date64 foundDate;
		while (ff.find()) {
			Date64 date = getBackupDate(re.getRE());
			if (date > foundDate) {
				foundDate = date;
				found = ff.found();
			}
		}
		if (time) *time = foundDate;
		return found;
	}


	void FileBin::backupFile(const std::string& filename, bool move) {
		if (filename.empty()) throw DTException(errBadParameter);
		if (move) {
			MoveFileEx(filename.c_str(), makeBackupFilename(filename).c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		} else {
			CopyFile(filename.c_str(), makeBackupFilename(filename).c_str(), false);
		}
	}

	void FileBin::backupFile(bool move) {
		if (Time64(true) - this->_table->_timeLastBackup < Time64(minimumBackupPeriod))
			return;
		this->_table->_timeLastBackup.now();
		this->backupFile(_fileName, move);
	}

	struct BackupFound {
		BackupFound(const Date64& date = Date64(), const std::string& file=""):date(date), file(file) {
		}
		bool operator == (const std::string& file) const {
			return this->file == file;
		}
		Date64 date;
		std::string file;
	};

	struct FileBackups {
		std::list<std::string> files; // wszystkie backupy
		std::vector<BackupFound > found; // backupy wg. granic
	};

	void FileBin::cleanupBackups(const std::string& filename) {


		std::map<std::string, FileBackups> files;
		std::vector<Date64> range;

		range.push_back(Time64(true) - (30 * 24 * 60 * 60)); // sprzed miesiaca
		range.push_back(Time64(true) - (7 * 24 * 60 * 60)); // sprzed tygodnia
		range.push_back(Time64(true) - (24 * 60 * 60)); // z wczoraj
		range.push_back(Time64(true) - (6 * 60 * 60)); // sprzed 6 godzin
		range.push_back(Time64(true)); // najnowsze

		bool all = isDirectory(filename.c_str());
		FindFileFiltered ff((all ? filename : getFileDirectory(filename, true)) + "\\*.bak");
		ff.setFileOnly();
		FileFilter_RegEx& re = *(new FileFilter_RegEx("/^(" + (all ? std::string(".+\\.dtb") : RegEx::addSlashes(::Stamina::getFileName(filename))) + ").(\\d+)-(\\d+)-(\\d+) (\\d+)-(\\d+)-(\\d+).bak$/i"));
		ff.addFilter(re);
		while (ff.find()) {
			FileBackups& file = files[re->getSub(1)];
			if (file.found.empty()) {
				file.found.resize(range.size());
			}
			file.files.push_back(ff->getFileName());
			Date64 date = getBackupDate(re.getRE());

			for (unsigned int i = 0; i < range.size(); ++i) {
				/* przegl¹damy granice od najstarszej wiêc wiêc pasuje nam ta, która jest <= aktualnej granicy (bêdzie od razu > od poprzedniej). W samej granicy wybieramy t¹ która jest najnowsza... */
				Date64& rangeDate = range[i];
				if (date <= range[i]) {
					if (date > file.found[i].date) {
						file.found[i] = BackupFound(date, ff->getFileName());
					}
					break;
				}
			}
		}

		// usuwamy wszystko co jest na liscie plików a nie ma na liœcie zaakceptowanych
		for (std::map<std::string, FileBackups>::iterator it = files.begin(); it != files.end(); ++it) {
			FileBackups& backups = it->second;
			for (std::list<std::string>::iterator file = backups.files.begin(); file != backups.files.end(); ++file) {
				if (std::find(backups.found.begin(), backups.found.end(), *file) == backups.found.end()) { // nie ma go na liœcie znalezionych wiêc usuwamy...
					DeleteFile(file->c_str());
				}
			}
		}
	}

	void FileBin::restoreBackup(const std::string& filename) {
		if (filename.empty()) throw DTException(errBadParameter);
		if (fileExists(filename.c_str()) == false) throw DTException(errFileNotFound);
		std::string original = RegEx::doGet("/^(.+\\.dtb).\\d+-\\d+-\\d+ \\d+-\\d+-\\d+.bak$/i", filename.c_str(), 1);
		// plik nie jest backupem zadnego dtb
		if (original.empty()) throw DTException(errBadParameter);
		std::string target = original + Date64(true).strftime(".%m-%d-%Y %H-%M-%S.restored");
		MoveFileEx(original.c_str(), target.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		MoveFileEx(filename.c_str(), original.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);

	}



	bool FileBin::restoreLastBackup(const std::string& filename) {
		// szukamy backupów
		FindFile::Found found = DT::findLastBackup(filename.empty() ? this->_fileName : filename);
		if (found.empty()) return false;
		restoreBackup(found.getFileName());
		return true;
	}

	Date64 FileBin::findLastBackupDate(const std::string& filename) {
		Date64 date;
		DT::findLastBackup(filename.empty() ? this->_fileName : filename, &date);
		return date;
	}
	std::string FileBin::findLastBackupFile(const std::string& filename) {
		return DT::findLastBackup(filename.empty() ? this->_fileName : filename).getFileName();
	}



	} }