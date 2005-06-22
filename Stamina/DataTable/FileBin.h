#pragma once
#ifndef __DT_FILEBIN__
#define __DT_FILEBIN__

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#include <io.h>
#include "FileBase.h"
#include <Stamina\Helpers.h>

namespace Stamina { namespace DT {



	const char binRowSeparator = '\n';
	const char binColumnSeparator = '\n';
	const char binVersionMaj = '3';
	const char binVersionMin = '5';


	class FileBin : public FileBase {

	public:

		friend class TestFileBin;

		enum enFileFlags {
			fflagFragmented = 0x10000,
			/**Uses crypt procedures on all fields*/
			fflagCryptAll = 0x20000,

			fflagNone = 0,
		};

		enum enDataFlags {
			dflagNone = 0,

			dflagLastId = 1,
			dflagPasswordDigest = 2,
			dflagDBId = 4,

			dflagParams = 8, /*TODO: */
			dflagCreated = 0x10,
			dflagModified = 0x20,
			dflagLastBackup = 0x40,
			dflagPassSalt = 0x80,
			dflagXorSalt = 0x100,

			basicDataFlags = dflagLastId | dflagPasswordDigest |  dflagCreated | dflagModified | dflagLastBackup | dflagPassSalt | dflagXorSalt,

			deprecatedDataFlags = dflagDBId,

			requiredDataFlags = dflagLastId | dflagPasswordDigest | dflagPassSalt | dflagXorSalt,
		};

		enum enRowDataFlags {
			rdflagRowId = 1,
			rdflagNone = 0,
		};

		enum enPositionOrigin {
			fromCurrent = SEEK_CUR,
			fromEnd = SEEK_END,
			fromBeginning = SEEK_SET
		};

	public:

		FileBin();
		FileBin(DataTable& table);
		~FileBin();

		void init();

		virtual void reset();


		void open (const std::string& fn , enFileMode mode) throw (...);
		void close ();

		virtual void readRows(bool skipFailed) throw (...); // wczytuje wiersze

		enFileFlags getFileFlags() {
			return _fileFlag;
		}
		/**Sets file flags.
		@warning You are responsible to set them wisely. Flags are always stored in the file when closing (together with row count). Extreme caution should be taken when appending data to the file.
		*/
		void setFileFlag(enFileFlags flag, bool setting) {
			if (setting)
				_fileFlag = (enFileFlags) (_fileFlag | flag);
			else
				_fileFlag = (enFileFlags) (_fileFlag & (~flag));
		}
		bool hasFileFlag(enFileFlags flag) {
			return (_fileFlag & flag) != 0;
		}

		std::string getOpenedFileName() {
			if (! _temp_fileName.empty())
				return _temp_fileName;
			else
				return _fileName;
		}

		/**Returns true if file is in the version that supports new crypt functions*/
		inline bool versionNewCrypt() {
			if (_verMaj > '3') return true;
			if (_verMaj < '3') return false;
			return _verMin >= '5';
		}

		inline bool isReadable() {
			return !_recreating && (_opened & (fileRead | fileAppend));
		}
		inline bool isCreatingNewFile() {
			return _recreating;
		}


	protected:

		enResult readRow(tRowId row) throw (...) {
			return readPartialRow(row, 0);
		}

		void writeRow(tRowId row) throw (...);

		virtual void writeHeader() throw (...);

		virtual void readHeader() throw (...);

		void writeDescriptor() throw (...);

		void readDescriptor() throw (...);

		enResult readPartialRow(tRowId row , tColId* columns) throw (...);

		bool findNextRow();

		void seekToBeginning() {
			this->setFilePosition(0, fromBeginning);
		}
		void seekToEnd() {
			this->setFilePosition(0, fromEnd);
		}

		void writeState() throw (...);
		//void readCount() throw (...);

		void setErasedRow(bool overwrite=true , int testIndex=0) throw (...);


		void setFilePosition(int pos , enPositionOrigin origin) throw(...) {
			if (fseek(_file , pos , origin))
				throw DTFileException();
		}

		/**Checks table's password digest with file's one*/
		virtual bool isAuthenticated();


		/**Generates new password digest.
		Should be used only when creating new file.
		@warning It's already being called inside writeHeader!
		*/
		void generatePasswordDigest(bool newSalt);

		/**Generates xor mask digest.
		Should be called after loading (with newSalt==false),
		and before saving to the new file (with newSalt==true).
		@warning It's already being called inside readHeader && writeHeader!
		*/
		void generateXorDigest(bool newSalt);


		inline void readData(void* buffer, int size, unsigned int* decrement = 0) throw(...) {
			if (fread(buffer, size, 1, _file) < 1) {
				throw DTFileException();
			}
			if (decrement)
				*decrement -= size;
		}

		inline void writeData(const void* buffer, int size, unsigned int* increment = 0) throw(...) {
			if (fwrite(buffer, size, 1, _file) < 1) {
				throw DTFileException();
			}
			if (increment)
				*increment += size;
		}

		inline std::string readString(unsigned int* decrement = 0) throw(...) {
			unsigned int size;
			readData(&size, 4, decrement);
			if (ftell(_file) + size > _fileSize) throw DTException(errBadFormat);
			std::basic_string<char> s;
			readData(stringBuffer(s, size), size, decrement);
			stringRelease(s, size);
			return s;
		}

		inline void writeString(const std::basic_string<char>& s, unsigned int* increment = 0) throw(...) {
			unsigned int length = s.length();
			writeData(&length, 4, increment);
			writeData(s.c_str(), s.length(), increment);
		}

		void readCryptedData(const Column& col, void* buffer, int size, unsigned int* decrement = 0) throw(...);

		void writeCryptedData(const Column& col, void* buffer, int size, unsigned int* increment = 0) throw(...);


		inline void updateFileSize() {
			_fileSize = _filelength(_file->_file);
		}


	public:

		bool useTempFile;
		bool warn;

	protected:

		FILE * _file;
		int _pos_data;
		int _pos_rows;
		int _pos_cols;
		int _pos_dataLastId;
		int _pos_state;
		int _storedRowsCount;
		//int mode;
		char _verMaj , _verMin;
		enFileFlags _fileFlag;
		unsigned int _dataSize;
		enDataFlags _dataFlag;

		bool _recreating;

		/**File's password digest used for authentication.*/
		MD5Digest _passwordDigest;
		unsigned int _passwordSalt;

		/**File's xor mask digest.*/
		MD5Digest _xorDigest;
		unsigned int _xorSalt;

		bool _temp_enabled;
		CStdString _temp_fileName; // Podczas uzywania tempa plik z ... tempem :)

		unsigned int _fileSize;
	};

} }

#endif
