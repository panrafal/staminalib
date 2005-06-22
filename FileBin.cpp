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
		std::string fn = fileToOpen;
		this->setWriteFailed( false );
		_storedRowsCount=0;
		
		this->_opened = fileClosed;
		if (mode & fileRead) {
			_table->_lastId = rowIdMin;
	        //_table->_db = -1;
		}

		fn = getFullPathName(fn.c_str());
		this->_fileName = fn;
	    
		if (useTempFile && (mode & fileWrite) && !(mode & fileRead)) {
			_temp_fileName = fn;
			int i = 0;
			do {
				_temp_fileName = fn + stringf(".[%d].tmp", ++i);
			} while ( ! _access(_temp_fileName , 0));
		} else {
			_temp_fileName = "";
		}

		bool fileExists = (_access(this->getOpenedFileName().c_str(),0) == 0);

		const char* openmode;
		// Dla Append, oraz Read+Write otwieramy w trybie read+write i jeœli jest taka potrzeba, tworzymy plik na nowo
		if (((mode & fileAppend) || (mode & fileWrite && mode & fileRead))) {
			openmode = fileExists ? "w+b" : "r+b";
			_recreating = (fileExists == false);
			_table->_timeModified.now();
		} else if (mode & fileWrite) {
			openmode = "wb";
			_recreating = true;
			_table->_timeModified.now();
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
		}

    }

    void FileBin::close () {
		if (!this->isOpened()) return;

		this->writeState();
        fclose(_file);
		
		if (this->isWriteFailed()) {
		    _opened = fileClosed;
#ifdef _WINDOWS_
			std::string msg = stringf("Wyst¹pi³ b³¹d podczas zapisywania danych!\r\nUszkodzona kopia znajduje siê w:\r\n\t%s%s" , _temp_enabled ? _temp_fileName.c_str() : _fileName.c_str(), _temp_enabled ? "\r\n\r\nOryginalny plik pozosta³ bez zmian..." : "\r\n\r\nB³¹d wyst¹pi³ podczas zapisywania oryginalnej kopii!");
			int r = MessageBox(0 , msg.c_str() , "B³¹d zapisu .DTB" , MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif
			return;
		}

        if (_temp_enabled) { // trzeba przerzuciæ tempa
            bool success = true;
			int retries = 0;
            while (1) {
				_unlink(_fileName.c_str());
				if (rename(_temp_fileName.c_str() , _fileName.c_str()) == 0)
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
        unsigned int dataLeft;
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
				std::string key = readString(&dataLeft);
				std::string value = readString(&dataLeft);
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

			_verMaj = DT::binVersionMaj;
			_verMin = DT::binVersionMin;

			writeData("DTBIN", 5); 

			writeData(&_verMaj, 1);
			writeData(&_verMin, 1);

			_pos_state = ftell(_file);

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
				writeData(_passwordDigest.getDigest(), 16, &_dataSize);
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
			_fcols.setColumn(id, type, 0, name.c_str());
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
				const Column& col = _fcols.getColumnByIndex(colIndex);
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

	enResult FileBin::readPartialRow(tRowId row , tColId* columns) {
		if (!isOpened()) throw DTException(errNotOpened);

		// Pokojowe wyjœcie - nie ma co czytaæ wiêc koñczymy
		if ( feof(_file) ) return resNothingToRead;
		// Od tego momentu wszystko czego nie da siê wczytaæ oznacza z³y format!

		row = _table->getRowPos(row);
	  
		//_table->notypecheck=1;  // wylacza sprawdzanie typow ...
      
		DataRow& rowObj = _table->getRow(row);
		
		// zapiujemy pozycjê wiersza
		rowObj._filePos = ftell(_file);

		// Znacznik rozpoczêcia nowego wiersza
		if (fgetc(_file) != '\n') throw DTException(errBadFormat);

		unsigned int rowSize;
		unsigned int rowDataSize;
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
            
			readData(&rowObj._flag, 4);

			// Sprawdzamy czy flaga jest równa -1 czyli czy element
			// nie jest oznaczony jako usuniêty
			if (rowObj._flag == -1) {
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
			if (id != rowObj.getId()) {
				if (_table->rowIdExists(id)) {
					id = _table->getNewRowId();
				}
				rowObj.setId(id);
			}
		} else {
			// w zasadzie nie ma potrzeby przydzielaæ nowego ID, bo jest ju¿ przydzielony przy okazji utworzenia wiersza... Poza tym ka¿dy szanuj¹cy siê DTB zawiera t¹ wartoœæ...
			// TODO: sprawdziæ!
		}

		// Pomijamy dane których nie rozumiemy...
		if (rowDataLeft > 0) setFilePosition(rowDataLeft, fromCurrent);

		// £adujemy dane
		for (unsigned int colIndex = 0; colIndex < _fcols.getColCount(); colIndex++) {
			const Column& col = _fcols.getColumnByIndex(colIndex);

			// skoro kolumna nie s³u¿y do zapisywania - nie mamy co wczytywaæ...
			if (col.hasFlag(cflagDontSave)) continue;

			// identyfikator kolumny w TABLICY
			tColId colId = col.getId();

			if (col.isIdUnique()) { 
				colId = _table->getColumns().getNameId(col.getName().c_str()); 
			}

			bool skip = (colId == colNotFound); // Czy OMIN¥Æ dane kolumny?

			const Column& tableCol = _table->getColumns().getColumn(colId);
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
						readCryptedData(col, &val, 4);
						rowObj.setByIndex(colIndex, (DataEntry)val);
					}
					break;
				case ctypeInt64:
					if (skip) {
						skipBytes = 8;
					} else {
						__int64 val;
						readCryptedData(col, &val, 8);
						rowObj.setByIndex(colIndex, (DataEntry)&val, true);
					}
					break;
				case ctypeString: {
					unsigned int length;
					readData(&length, 4);
					if (ftell(_file) + length > _fileSize)
						throw DTException(errBadFormat);
					if (skip) {
						skipBytes = length;
					} else if (length > 0) {
						char * buffer = new char [length + 1];
						buffer[length] = 0;
						readCryptedData(col, buffer, length);
						rowObj.setByIndex(colIndex, (DataEntry)buffer, true);
						delete [] buffer;
					} else {
						rowObj.setByIndex(colIndex, (DataEntry)"", true);
					}
					break;}
				case ctypeBin: {
					TypeBin bin;
					bin.buff = 0;
					readData(&bin.size, 4); // wczytujemy rozmiar
					if (ftell(_file) + bin.size > _fileSize)
						throw DTException(errBadFormat);
					if (skip) {
						skipBytes = bin.size;
					} else if (bin.size > 0) {
						bin.buff = new char [bin.size];
						readCryptedData(col, bin.buff, bin.size);
						rowObj.setByIndex(colIndex, (DataEntry)&bin, true);
						delete [] bin.buff;
					} else {
						rowObj.setByIndex(colIndex, (DataEntry)&bin, true);
					}
					break; }

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

			DataRow& rowObj = _table->getRow(row);

			unsigned int rowSize = 0;

			// rozmiar, flagi - od v2.0
			if (_verMaj > '1') {
				rowSize = 0x7FFFFFFF;
				writeData(&rowSize, 4);  //rowSize - placeholder
				rowSize = 0;
				enRowFlag rowFlags = rowObj.getFlags();
				writeData(&rowFlags, 4, &rowSize); // flag
				unsigned int dataSize = 8; // flag + lastId, na razie nie ma wiêcej
				writeData(&dataSize, 4, &rowSize);
				enRowDataFlags flags = rdflagRowId;
				writeData(&flags, 4, &rowSize);
				tRowId rowId = rowObj.getId();
				rowId = DataTable::unflagId(rowId);
				writeData(&rowId, 4, &rowSize);
			}
			for (unsigned int colIndex =0; colIndex < _fcols.getColCount(); colIndex++) {
				const Column& col = _fcols.getColumnByIndex(colIndex);

				if (col.hasFlag(cflagDontSave)) continue;

				tColId id = col.getId();
				if (col.isIdUnique()) { 
					id = _table->getColumns().getNameId(col.getName().c_str()); 
				}

				switch (col.getType()) {
					case ctypeInt: {
						int val = (int)rowObj.getByIndex(colIndex);
						writeCryptedData(col, &val, 4, &rowSize);
						break;}
					case ctype64: {
						__int64* val = (__int64*)rowObj.getByIndex(colIndex);
						if (!val) {
							// zapisujemy 0
							__int64 null = 0;
							writeCryptedData(col, &null, 8, &rowSize);
						} else {
							writeCryptedData(col, val, 8, &rowSize);
						}
						break;}
					case ctypeString: {
						char * val = (char *)rowObj.getByIndex(colIndex);
						unsigned int length = (val == 0 ? 0 : strlen(val));
						writeData(&length, 4, &rowSize);
						if (val && length > 0) {
							writeCryptedData(col, val, length, &rowSize);
						}
						break;}
					case ctypeBin: {
						TypeBin* val = (TypeBin*)rowObj.getByIndex(colIndex);
						if (val) {
							writeData(&val->size, 4, &rowSize);
							if (val->buff && val->size > 0) {
								writeCryptedData(col, val->buff, val->size, &rowSize);
							}
						} else {
							int size = 0;
							writeData(&size, 4, &rowSize);
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
				this->readRow(row);
			} else {
				while (1) {
					try {
						this->readRow(row);
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

	void FileBin::readCryptedData(const Column& col, void* buffer, int size, unsigned int* decrement) {
		this->readData(buffer, size, decrement);
		if (col.hasFlag(cflagXor) || this->hasFileFlag(fflagCryptAll)) {
			if (this->versionNewCrypt()) {
				xor2_decrypt(_xorDigest.getDigest(), (unsigned char*)buffer, size, ftell(_file));
			} else {
				xor1_decrypt(_table->getXor1Key(), (unsigned char*)buffer, size);				
			}
		}
	}

	void FileBin::writeCryptedData(const Column& col, void* buffer, int size, unsigned int* increment) {
		if (col.hasFlag(cflagXor) || this->hasFileFlag(fflagCryptAll)) {
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



	} }