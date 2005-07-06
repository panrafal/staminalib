/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once


namespace Stamina {

	
	class FindFile {
	public:

		enum enAttribute {
			attReadOnly = FILE_ATTRIBUTE_READONLY,
			attHidden = FILE_ATTRIBUTE_HIDDEN,
			attSystem = FILE_ATTRIBUTE_SYSTEM,
			attDirectory = FILE_ATTRIBUTE_DIRECTORY,
			attArchive = FILE_ATTRIBUTE_ARCHIVE, 
			attDevice = FILE_ATTRIBUTE_DEVICE, 
			attNormal = FILE_ATTRIBUTE_NORMAL,
			attTemporary = FILE_ATTRIBUTE_TEMPORARY,
			attSparse = FILE_ATTRIBUTE_SPARSE_FILE,
			attReparsePoint = FILE_ATTRIBUTE_REPARSE_POINT,
			attCompressed = FILE_ATTRIBUTE_COMPRESSED,
			attOffline = FILE_ATTRIBUTE_OFFLINE,
			attNotContentIndexed = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
			attEncrypted = FILE_ATTRIBUTE_ENCRYPTED,

		};

		class Found {
		public:
			Found() {
			}
			inline Found(const Found& b) {
				*this = b;
			}

			inline Found& operator = (const Found& b) {
				memcpy(&_data, &b._data, sizeof(_data));
			}

			inline WIN32_FIND_DATA* getDataRef() {
				return &_data;
			}

			inline const WIN32_FIND_DATA& operator -> () {
				return _data;
			}

			inline bool hasAttribute(enAttribute attr) const {
				return (_data.dwFileAttributes & attr) != 0;
			}

			inline int getAttributes() const {
				return _data.dwFileAttributes;
			}

			inline const FILETIME& getCreationTime() const {
				return _data.ftCreationTime;				
			}

			inline const FILETIME& getLastAccessTime() const {
				return _data.ftLastAccessTime;				
			}

			inline const FILETIME& getLastWriteTime() const {
				return _data.ftLastWriteTime;				
			}

			inline __int64 getFileSize() const {
				__int64 size;
				size = _data.nFileSizeHigh;
				size = size >> 32;
				size |= _data.nFileSizeLow;
				return size;
			}

			inline std::string getFileName() const {
				return _data.cFileName;
			}

			inline std::string getAlternateFileName() const {
				return _data.cAlternateFileName;
			}

			inline bool isParentPointer() const {
				return strcmp(_data.cFileName, ".") == 0 || strcmp(_data.cFileName, "..") == 0;
			}

			inline bool isDirectory() const {
				return this->hasAttribute(attDirectory);
			}

		private:
			WIN32_FIND_DATA _data;
		};

#ifdef _LIST_
		typedef std::list<Found> tFoundFiles;
#endif

	public:

		FindFile(const std::string& mask = "") {
			_file = 0;
			_require = 0;
			_exclude = 0;
			_includeParent = false;
			setMask(mask);
		}
		~FindFile() {
			setMask(""); // zamyka Find'a od razu
		}

		void setMask(const std::string& mask) {
			if (_file) {
				FindClose(_file);
				_file = 0;
			}
			_mask = mask;
		}

		void requireAttribute(enAttribute attr) {
			_require = _require | attr;
		}
		void excludeAttribute(enAttribute attr) {
			_exclude = _exclude | attr;
		}

		void setDirOnly() {
			requireAttribute(attDirectory);
		}
		void setFileOnly() {
			excludeAttribute(attDirectory);
		}

		void includeParent() {
			_includeParent = true;
		}


		/** Looks for first/next file. Returns false if nothing found */
		bool find();

#ifdef _LIST_
		tFoundFiles makeList() {
			tFoundFiles list;
			while (this->find()) {
				list.push_back(this->found());
			}
		}
#endif

		/** Returns information about found file */
		inline const Found& found() const {
			return _found;
		}

		inline const Found* operator -> () const {
			return &_found;
		}

		inline bool nothingFound() const {
			return _file == 0;
		}

	protected:

		virtual bool filter();

		int _require, _exclude;
		std::string _mask;
		HANDLE _file;
		Found _found;
		bool _includeParent;

	};



}