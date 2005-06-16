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


using namespace std;

namespace Stamina { namespace DT {

	FileBin::FileBin() { 
		init(); 
	}
	
	FileBin::FileBin(DataTable * t) {
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
		_pos_data = _pos_rows = _pos_cols = _fileFlag = 0;
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

	void FileBin::open (const std::string& fn , enFileMode mode) {
		this->close();

		this->setWriteFailed( false );
		_storedRowsCount=0;
		
		this->_opened = fileClosed;
		if (mode & fileRead) {
			_table->lastid = rowIdMin;
	        _table->dbID = -1;
		}

		fn = getFullPathName(fn);
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
		} else if (mode & fileWrite) {
			openmode = "wb";
			_recreating = true;
		} else {
			openmode = "rb";
			_recreating = false;
		}

		_file=fopen(this->getOpenedFileName().c_str(), openmode);

		if (_file && useTempFile && (mode & fileWrite)) {
			_temp_enabled = true;
		} else {
			_temp_enabled = false;
			_temp_fileName = "";
		}

		if (!_file) throw DTFileException();


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

		this->writeSize();
        fclose(_file);
		
		if (this->isWriteFailed()) {
		    _opened = fileClosed;
#ifdef _WINDOWS_
			std::string msg = string("Wyst¹pi³ b³¹d podczas zapisywania danych!\r\nUszkodzona kopia znajduje siê w:\r\n\t%s%s" , _temp_enabled ? _temp_fileName.c_str() : _filename.c_str(), _temp_enabled ? "\r\n\r\nOryginalny plik pozosta³ bez zmian..." : "\r\n\r\nB³¹d wyst¹pi³ podczas zapisywania oryginalnej kopii!");
			int r = MessageBox(0 , msg.c_str() , "B³¹d zapisu .DTB" , MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif
			return;
		}

        if (_temp_enabled) { // trzeba przerzuciæ tempa
            bool success = true;
			int retries = 0;
            while (1) {
				_unlink(_fileName);
				if (rename(_temp_fileName , _fileName) == 0)
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
		if (_verMaj > versionMajor) throw DTException(errBadVersion);

		// £adujemy drugi cz³on wersji, oraz flagi (dostêpne od v2.0)
	    if (_verMaj > '1') {
			readData(&_verMin , 1);
			readData(&_fileFlag , 4);
		} else {
			_verMin = 0; 
			flag = 0;
		}

		// £adujemy liczbê wierszy
		readData(&_storedRowsCount, 4);  // rowc
		_table->_size = _storedRowsCount;
      
		int csize , a , b;
		_table->_lastId = DT_ROWID_MIN;
		md5digest[0]=0;
		table->dbID = -1;

		_dataFlag = dflagNone;
        int dataLeft;
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
			readData(&_table->_lastId , 4, &dataLeft);
		} else {
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
			_table->_timeCreated = 0;
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

		// generujemy nowe has³o i maskê xor z nowymi saltami
		this->generatePasswordDigest(true);
		this->generateXorDigest(true);

		_verMaj = versionMajor;
		_verMin = versionMinor;

		writeData("DTBIN", 5); 

		writeData(&_verMaj, 1);
		writeData(&_verMin, 1);

		writeData(&_fileFlag, 4); // flag

		writeData(&(_table->getRowCount()), 4);    // rowc

		_dataSize = 0x7FFFFFFF;
		// placeholder do zapisania trochê póŸniej. Specjalnie zapisujemy du¿¹ wartoœæ, ¿eby w razie pozostawienia tego w takim stanie w pliku spowodowaæ "bezpieczny" b³¹d podczas wczytywania
		writeData(&_dataSize, 4);

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
			writeData(&_table->lastid, 4, &_dataSize);
		}
		if (_dataFlag & dflagPasswordDigest) {
			writeData(_passwordDigest.getDigest(), 16, &_dataSize);
		}

		if (_dataFlag & dflagParams) {
			writeData(&(_table->getParamsMap().size()), 4, &_dataSize);
			for (DataTable::tParams::iterator it = _table->getParamsMap().begin(); it != _table->getParamsMap().end(); it++) {
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
        this->setFilePosotion(_dataSize, fromCurrent);
	}

// DESCRIPTOR ** READ -------------------------------------------------------

    int FileBin::freaddesc() {
      if (!opened) return 1;

      fread(&csize, 4, 1 , file);
      fcols.setcolcount(csize);
      for (int i=0; i<csize; i++) { // columns definitions
        fread(&a, 4, 1 , file);
        fread(&b, 4, 1 , file);
        int id = a;
        int type = b;
        char * name = 0;
        if (_verMaj>'2') { // version 3x only
            char c;
            fread(&c , 1 , 1 , file);
            if (c) {
                 name = new char [c+1];
                 fread(name , c , 1 , file);
                 name[c]=0;
            }
            fread(&b , 4 , 1 , file);
            if (b) fseek(file , b , SEEK_CUR); // We have to pass unprocessed data.
        }
        fcols.setcol(id , type , 0 , name?name:"");
        if (name) delete [] name;
      }
      pos_row = ftell(file);
      return 0;
    }

// DESCRIPTOR ** WRITE-------------------------------------------------------

    int FileBin::fwritedesc() {
      if (!opened) return 1;

      b = fcols.getcolcount();
	  if (fwrite(&b, 4, 1 , file) != 1)    // colc
	      goto writefailed;

      for (i=0; i < fcols.getcolcount() ; i++) {  // Columns definitions
        Column * cdi = &fcols.cols[i];
        fwrite(&cdi->id, 4, 1 , file);   //id
		int type = cdi->type & (~DT_CF_NOSAVEFLAGS); 
        fwrite(&type, 4, 1 , file);   //type
        char c= (cdi->name.size() > 255)?255:cdi->name.size();
        fwrite(&c , 1 , 1 , file); // name length
        if (c) fwrite(cdi->name.c_str() , c , 1 , file); // name
        b = 0;
		if (fwrite(&b , 4 , 1, file) != 1) // 0-length data field. For future use maybe.
	        goto writefailed;

      }
      pos_row = ftell(file);
      return 0;
writefailed:
	  this->write_failed = true;
	  return 2;
    }

// SIZE ** READ -------------------------------------------------------------

    int FileBin::freadsize() {
      if (!opened) return 1;
      fseek(file , (_verMaj>'1')?11:6 , SEEK_SET);
      fread(&_storedRowsCount, 4, 1 , file);
//      MessageBox(0 , inttoch(_storedRowsCount) , "" , 0);
      return 0;
    }

// SIZE ** WRITE ------------------------------------------------------------

	int FileBin::fwritesize() {
      if (!opened) return 1;
      if (mode & (DT_WRITE | DT_APPEND) && _storedRowsCount != -1) {
        fseek(file , (_verMaj>'1')?11:6 , SEEK_SET);
        if (fwrite(&_storedRowsCount, 4, 1 , file) != 1)
		    goto writefailed;
        if (table && _verMaj>'1' && (_dataFlag & DT_BIN_DFID) && (_dataSize >= 8)) {
          fseek(file , 23 , SEEK_SET);
		  if (fwrite(&table->lastid , 4 , 1 , file) != 1)
		      goto writefailed;
        }
      }
      return 0;
writefailed:
	  this->write_failed = true;
	  return 2;
    }



// ROW ** READ --------------------------------------------------------------

    int FileBin::freadpartialrow(int row , int * columns) {
        start:
      if (feof(file)) return 1;
      row = DT_GETPOS(table , row);
      if (!opened) return 1;
	  size_t filesize = _filelength(file->_file);
      table->notypecheck=1;  // wylacza sprawdzanie typow ...
      DataRow * rowObj = table->rows[row];
      rowObj->pos = ftell(file);
      if (fgetc(file) != '\n') return 1;
      if (feof(file)) return 1;
      int b , ctype , cflag , id;
      Td64 d;
      char * c;
      table->lastid++;
      rowObj->id = table->lastid;
      int siz1 , siz2 , p;
      if (_verMaj > '1') {
        fread(&siz1 , 4 , 1 , file); // size
        p = ftell(file); // pocz¹tek danych...
		if (p + siz1 + 4 > filesize)
			return 1;
        if (fseek(file , siz1 , SEEK_CUR))
			return 1;
        if (!fread(&siz2 , 4 , 1 , file)) // size2
			return 1;
        fseek(file , p , SEEK_SET); // Wracamy do pozycji
        if (siz1 != siz2) 
			return 1; // Oba rozmiary nie s¹ zgodne!!!
        if (!fread(&rowObj->flag  , 4 , 1 , file))
			return 1; // flag
        // Sprawdzamy czy flaga jest równa -1 czyli czy element
        // nie jest oznaczony jako usuniêty
        if (rowObj->flag == -1) {
            fseek(file , siz1 , SEEK_CUR); // przesuwamy siê o p + size + 4 (rozmiar, wczytany jako flaga)...
            table->lastid --;
            goto start; // Wczytujemy nastêpn¹ pozycjê
        }

        int dsiz;
        fread(&dsiz , 4 , 1 , file); // dsiz
		if ((p + siz1) < ftell(file) + dsiz)
			return 1;
        if (dsiz) {
          int _dataFlag;
          fread(&_dataFlag , 4 , 1 , file); // _dataFlag
          dsiz -=4;
          if (_dataFlag & DT_BIN_DFID)
               {fread(&rowObj->id , 4 , 1 , file);
                if (rowObj->id > table->lastid) table->lastid = rowObj->id+1;
                table->lastid--;
                dsiz -=4;}
        }

        if (dsiz>0) fseek(file , dsiz , SEEK_CUR);

      }

      for (unsigned int i =0; i<fcols.cols.size(); i++) {
        cflag = fcols.cols[i].type;
        ctype = cflag & 0xFF;
        id = fcols.cols[i].id;
        if (id & DT_COLID_UNIQUE) { 
            id = table->cols.getnameid(fcols.cols[i].name.c_str()); 
        }
        if (cflag & DT_CT_DONTSAVE) continue;
		  bool skip = false; // Czy OMIN¥Æ kolumnê?
		  if (columns) { // szukamy, czy ID aktualnej, jest na liœcie, jak nie to omijamy...
			  skip = true;
			  int i = 0;
			  do {
				  if (id == columns[i]) {
					  skip = false;
					  break;
				  }
				  i++;
			  } while (columns[i]);
		  }

		switch (ctype) {
          case DT_CT_INT:
			  if (skip)
				  fseek(file , 4 , SEEK_CUR);
			  else {
				fread(&b, 4, 1 , file);
				table->setint(row , id , b);
			  }
              break;
          case DT_CT_64:
			  if (skip)
				  fseek(file , 8 , SEEK_CUR);
			  else {
				fread(&d , 8 , 1 , file);
				table->set64(row , id , d);
			  }
				break;
          case DT_CT_PCHAR: case DT_CT_STRING:
            fread(&b, 4, 1 , file);
			if ((p + siz1) < ftell(file) + b)
				return 1;
			if (skip)
				fseek(file , b , SEEK_CUR);
			else {
				c=new char [b+1];
				c[b]='\0';
				if (b) fread(c , b , 1 , file);
				if (cflag & DT_CT_CXOR) {
				sc_decrypt((unsigned char*)table->cxor_key , (unsigned char*)c , b);
				}
				if (ctype == DT_CT_PCHAR) table->setch(row , id , c);
				else table->setstr(row , id , c);
				delete [] c;
			}
            break;
          case DT_CT_BIN:{
            TypeBin bin;
            bin.buff = 0;
            fread(&bin.size, 4, 1 , file); // wczytujemy rozmiar
			if ((p + siz1) < ftell(file) + bin.size)
				return 1;
			if (skip)
				fseek(file , bin.size , SEEK_CUR);
			else {
				if (bin.size) {
					bin.buff = new char [bin.size];
					fread(bin.buff , bin.size , 1 , file);
				}
				table->setbin(row , id , &bin);
				delete [] bin.buff;
			}
            break;}

        }
        if (fgetc(file) != '\t')
			return 1;
      }

	  if (_verMaj > '1') {
        fread(&siz2 , 4 , 1 , file); // size
		if (siz1 != siz2) return 1;
	  }
      table->notypecheck=0;
      return 0;
    }


// ROW ** WRITE -------------------------------------------------------------

    int FileBin::fwriterow(int row) {
      row = DT_GETPOS(table , row);
      if (!opened) return 1;
      _storedRowsCount++;
      if (fputc('\n' , file) == EOF)
		  goto writefailed;
      int b , ctype , cflag , id;
      char * c , * c2;
      Td64 * d;
      int bsize = 0;
      int pos_size = ftell(file);
      b = 0;
      if (_verMaj > '1') {
        fwrite(&b , 4 , 1, file);  //_storedRowsCount
        fwrite(&table->rows[row]->flag , 4 , 1 , file); // flag
        bsize+=4;
        b = 8;
        fwrite(&b , 4 , 1 , file); // dsiz , dat
        bsize+=b;
        if (b>0) {
          b = DT_BIN_DFID;
          fwrite(&b , 4 , 1 , file);
          if (fwrite(&table->rows[row]->id , 4 , 1 , file) != 1)
			  goto writefailed;
        }
      }
      for (unsigned int i =0; i<fcols.cols.size(); i++) {
        cflag = fcols.cols[i].type;
        ctype = cflag & 0xFF;
        id = fcols.cols[i].id;
        if (id & DT_COLID_UNIQUE) { 
            id = table->cols.getnameid(fcols.cols[i].name.c_str()); 
        }
        if (cflag & DT_CT_DONTSAVE) continue;
        switch (ctype) {
          case DT_CT_INT: b=(int)table->get(row , id);
            fwrite(&b, 4, 1 , file);
            bsize +=4;
            break;
          case DT_CT_64:
            d = (Td64*)table->get(row , id);
            if (!d) {Td64 db; fwrite(&db , 8 , 1 , file);bsize+=8;break;}
            fwrite(d , 8 , 1 , file);
            bsize+=8;
            break;
          case DT_CT_PCHAR: case DT_CT_STRING:
            c=(ctype==DT_CT_PCHAR)? (char *)table->get(row , id)
                               : (char*)(*(string *)table->get(row , id)).c_str()
                               ;
            if (!c) {b=0;fwrite(&b, 4, 1 , file);bsize+=4;break;}
            b=strlen(c);
            if (cflag & DT_CT_CXOR) {
               c2 = strdup(c);
               sc_encrypt((unsigned char*)table->cxor_key , (unsigned char*)c2 , b);
			} else {c2 = c;}
            fwrite(&b, 4, 1 , file);
            fwrite(c2 , b , 1 , file);
            if (cflag & DT_CT_CXOR) {
               free(c2);
            }
            bsize += 4+b;
            break;
          case DT_CT_BIN: {
              TypeBin bin = table->getbin(row , id);
              fwrite(&bin.size, 4, 1 , file);
              if (bin.size) fwrite(bin.buff , bin.size , 1 , file);
              bsize+=4+bin.size;
              break;}
        }
		if (fputc('\t' , file) == EOF) // sprawdzamy tylko ostatnie write'y
		    goto writefailed;
        bsize+=1;
      }
      if (_verMaj > '1') {
        bsize+=4;
        fwrite(&bsize , 4 , 1, file); // size
        b = ftell(file);
        fseek(file , pos_size , SEEK_SET);
		if (fwrite(&bsize , 4 , 1, file) != 1) // size
		    goto writefailed;

        fseek(file , b , SEEK_SET);
      }
      return 0;
writefailed:
	  this->write_failed = true;
	  return 2;
    }


// ----------------------------------------------------------------------




    int FileBin::fseterasedrow(bool overwrite , int testIndex) {
        if (feof(file)) return 1;
        if (!opened) return 1;
        if (fgetc(file) != '\n') return 1; // wczytuje '\n'
        if (feof(file)) return 1;
        if (_verMaj && _verMaj < '3') return 1;
        int siz1 , siz2;
        long pos;
        // Najpierw sprawdza czy zgadza siê rozmiar...
        fread(&siz1 , 4 , 1 , file); // size
        pos = ftell(file); // Pozycja zaraz za pierwszym SIZE'em
        fseek(file , siz1 , SEEK_CUR);
        fread(&siz2 , 4 , 1 , file); // size2
        if (feof(file) || siz1 != siz2) return 1; // Oba rozmiary nie s¹ zgodne!!!
        // Sprawdzamy index jeœli jest taka potrzeba
        if (testIndex) {
            fseek(file , pos + 4 , SEEK_SET); // Czytamy DSize
            int dSize=0;
            fread(&dSize , 4 , 1 , file);
            if (dSize >= 8) { // Potrzebujemy równie¿ flagê...
                int dFlag = 0;
                fread(&dFlag , 4 , 1 , file);
                if (dFlag & DT_BIN_DFID) {
                    int dID = 0;
                    fread(&dID , 4 , 1 , file);
                    if (dID != DT_UNMASKID(testIndex)) return 1; // Z³y index!
                }
            }
        }
        if (this->_storedRowsCount != -1 && this->_storedRowsCount) { // A mo¿e ju¿ jest ustawione?
            fseek(file , pos , SEEK_SET); // Wracamy do pozycji
            int oldFlag = 0;
            fread(&oldFlag , 4 , 1 , file);
            if (oldFlag == -1) return 0;
            this->_storedRowsCount --;
        } else this->_storedRowsCount = -1;

        fseek(file , pos , SEEK_SET); // Wracamy do pozycji
        int newFlag = -1;
		if (fwrite(&newFlag  , 4 , 1 , file) != 1) // Zapisujemy flagê
			goto writefailed;
        if (overwrite) {
            // Zapisujemy 
            char * buff = new char [100];
            memset(buff , 0 , 100);
            size_t size = siz1 - 4;
            size_t i = 0;
            while (i < size) {
                if (!fwrite(buff , min(100 , size-i) , 1 , file)) break;
                i+=min(100 , size-i);
            }
            delete [] buff;
        }
        // Zaznaczamy flage w deskryptorze
        fseek(file , 7 , SEEK_SET);
        int flag;
        fread(&flag , 4 , 1 , file);
        flag |= DT_BINTF_FRAGMENTED;
        fseek(file , 7 , SEEK_SET);
		if (fwrite(&flag , 4 , 1, file) != 1)
			goto writefailed;
        fseek(file , pos + siz1 + 4 , SEEK_SET); // Skaczemy dalej
        return 0;
writefailed:
	  this->write_failed = true;
	  return 2;

    }



// READ




	int FileBin::ffindnextrow() { // przechodzi do nastêpnej linijki (w razie gdy freadrow wywali b³¹d)
		if (_verMaj <= '1' || feof(file))
			return 1;
		//size_t filesize = _filelength(file->_file);
		while (!feof(file)) {
			// szukamy '\n'
			if (getc(file) == '\n') {
				fseek(file , -1 , SEEK_CUR);
				return 0; // znalaz³em potencjalny nowy wpis... freadrow zajmie siê sprawdzaniem...
			}
		}
		return 1;
	}


    int FileBin::readrows() {
//      int pos = ftell(file);
//      freadsize();
//      fseek(file , pos , SEEK_SET);
        table->clearrows();
        int i;
        while (1 && !feof(file))
        { 
            i = table->addrow(0);
			while (1) {
				table->rows[i]->pos = ftell(file);
				if (freadrow(i)) {
					if (!ffindnextrow())
						continue; // próbujemy nastêpny...
					else {
						table->deleterow(i);
						return 0;
					}
				} else {
					break; // jest ok, nastêpny...
				}
			}
		
		}
        return 0;
    }




	} }