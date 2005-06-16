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

#include "FileBase.h"
#include <Stamina\Helpers.h>

namespace Stamina { namespace DT {



	const char binRowSeparator = '\n';
	const char binColumnSeparator = '\n';
	const char binVersionMaj = '3';
	const char binVersionMin = '5';


	class FileBin : public FileBase {

	public:

		enum enFileFlags {
			fflagFragmented = 0x10000,
			/**Uses crypt procedures on all fields*/
			fflagCryptAll = 0x20000,
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

		enum enPositionOrigin {
			fromCurrent = SEEK_CUR,
			fromEnd = SEEK_END,
			fromBeginning = SEEK_SET
		};

	public:

		FileBin();
		FileBin(DataTable * table);
		~FileBin();

		void init();

		virtual void reset();


		void open (const std::string& fn , enFileMode mode) throw (...);
		void close ();

		virtual void readRows() throw (...); // wczytuje wiersze

	protected:

		bool readRow(int row) throw (...) {
			return readPartialRow(row, 0);
		}

		void writeRow(int row) throw (...);

		virtual void writeHeader() throw (...);

		virtual void readHeader() throw (...);

		void writeDescriptor() throw (...);

		void readDescriptor() throw (...);

		bool readPartialRow(int row , int * columns) throw (...);

		bool findNextRow();

		void seekToBeginning() {
			this->setFilePosition(0, fromBeginning);
		}
		void seekToEnd() {
			this->setFilePosition(0, fromEnd);
		}

		void writeSize() throw (...);
		void readSize() throw (...);

		void setErasedRow(bool overwrite=true , int testIndex=0) throw (...);


		void setFilePosition(int pos , enPositionOrigin origin) {
			fseek(_file , pos , origin);
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

		std::string getOpenedFileName() {
			if (! _temp_filename.empty())
				return _temp_filename;
			else
				return _fileName;
		}

		/**Returns true if file is in the version that supports new crypt functions*/
		inline bool versionNewCrypt() {
			if (_verMaj > '3') return true;
			if (_verMak < '3') return false;
			return _verMin >= '5';
		}

		inline bool isReadable() {
			return !_recreating && (_opened & (fileRead | fileAppend));
		}
		inline bool isCreatingNewFile() {
			return _recreating;
		}

		inline void readData(void* buffer, int size, int* decrement = 0) throw(...) {
			if (fread(buffer, size, 1, _file) < 1) {
				throw DTFileException();
			}
			if (decrement)
				*decrement -= size;
		}

		inline void writeData(void* buffer, int size, int* increment = 0) throw(...) {
			if (fread(buffer, size, 1, _file) < 1) {
				throw DTFileException();
			}
			if (increment)
				*increment += size;
		}

		inline std::string readString(int* decrement = 0) throw(...) {
			int size;
			readData(&size, 4, decrement);
			std::string<char> s;
			readData(stringBuffer(s, size), size, decrement);
			stringRelease(s, size);
			return s;
		}

		inline void writeString(const std::string<char>& s, int* increment = 0) throw(...) {
			writeData(&(s.length()), 4, increment);
			writeData(s.c_str(), s.length(), increment);
		}

	public:

		bool useTempFile;
		bool warn;

	protected:

		FILE * _file;
		int _pos_data;
		int _pos_rows;
		int _pos_cols;
		int _storedRowsCount;
		//int mode;
		char _verMaj , _verMin;
		int _fileFlag;
		int _dataSize;
		int _dataFlag;

		bool _recreating;

		/**File's password digest used for authentication.*/
		MD5Digest _passwordDigest;
		unsigned int _passwordSalt;

		/**File's xor mask digest.*/
		MD5Digest _xorDigest;
		unsigned int _xorSalt;

		bool _temp_enabled;
		CStdString _temp_fileName; // Podczas uzywania tempa plik z ... tempem :)
	};

} }

#endif
