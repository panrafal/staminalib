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

#include "DataTable.h"


namespace Stamina { namespace DT {

	enum enFileMode {
		fileRead = 1,
		fileWrite = 2,
		fileAppend = 4,
	};


	class FileBase {
	public:
		//   virtual

		FileBase();
		FileBase(DataTable * table);
		~FileBase();

		/**Assigns a table to this file handler*/
		void assign(DataTable * table);

		inline bool isWriteFailed() {
			return _writeFailed;
		}
		inline void setWriteFailed(bool state) {
			_writeFailed = state;
		}

		/**Loads data from specified file.
		Uses only column descriptor from assigned table.
		*/
		virtual enResult load (const std::string& fn = "", bool loadColumns = false); // wgrywa caly plik
		/**Loads all data from specified file.
		Uses merged column descriptors from assigned table and specified file.
		*/
		virtual enResult loadAll (const std::string& fn = "") {
			return this->load(fn, true);
		}

		/**Stores all information from assigned table. The file is recreated.*/
		virtual enResult save (const std::string& fn = ""); // zapisuje caly plik

		/**Appends table data to specified file using only file's column descriptor.*/
		virtual enResult append (const std::string& fn = ""); // dopisuje elementy

		// placeHolders

		/**Opens file in specified mode*/
		virtual void open (const std::string& fn , enFileMode mode)=0 throw;
		virtual void close ()=0; // zamyka plik
		//virtual examine() {return 0;}; // Zapisuje lokalizacje wierszy
		//virtual examinePos(tRowId row) {return 0;}; // zapisuje lokalizacje wiersza
		//virtual int readRow(int row)=0; // wczytuje wiersz
		//virtual freeRow(int row)=0; // zwalnia wiersz

		/**Reads all rows from file*/
		virtual void readRows()=0 throw; // wczytuje wiersze

		//virtual int next() {return 0;} // przesuwa sie na nastepny
		//virtual int prev() {return 0;} // przesuwa sie na poprzedni

	protected:

		/**Reads row from current position in file and stores it under @a row
		If there's nothing to read returns false instead of throwing exception.
		*/
		virtual bool readRow(int row)=0 throw; // wczytuje wiersz na aktualnej pozycji do row

		/**Stores data under @a row into current position in file*/
		virtual void writeRow(int row)=0 throw;

		/**Stores column descriptor*/
		virtual void writeDescriptor()=0 throw;

		/**Reads column descriptor*/
		virtual void readDescriptor()=0 throw;

		/**Reads only specified rows from current position in file and stores them under @a row.
		@param columns - null terminated list of columns
		*/
		virtual bool readPartialRow(int row , int * columns)=0 throw;

		/**Goes to next row in file*/
		virtual bool findNextRow()=0; // przechodzi do nastêpnej linijki (w razie gdy freadrow wywali b³¹d)


		virtual void seekToBeginning()=0;
		virtual void seekToEnd()=0;


	protected:
		DataTable * _table;
		bool _opened;
		ColumnsDesc _fcols;
		std::string _fileName;
		bool _writeFailed;

	};


} }

#endif