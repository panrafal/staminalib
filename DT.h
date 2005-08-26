#pragma once
#ifndef __DT_DT__
#define __DT_DT__

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

/**@file All basic definitions needed to work with DataTable

*/

#include <Stamina\Exception.h>

namespace Stamina { namespace DT {


	const unsigned int rowIdFlag = 0x40000000;
	const unsigned int rowIdMax = 0x3FFFFFFF;
	const unsigned int rowIdMin = 0x1;

	const unsigned int colIdUniqueFlag = 0x00800000;


#define DT_CC_RESIZE 1
#define DT_CC_FILL 2


#define DT_FT_TEXT 0
#define DT_FT_BINARY 1
//#define DT_FT_AUTO -1

	enum enError {
		errSuccess = 0,
		success = 0,

		errNoColumn = 100,
		errNoRow = 200,
		errBadType = 300,

		errFileNotFound = 0x1000,
		errNotInitialized = 0x1001,
		errNotAuthenticated = 0x1002,
		errNotOpened = 0x1003,
		errBadFormat = 0x1004,
		errBadVersion = 0x1005,
		errRowNotFound = 0x1006,
		errBadParameter = 0x1007,
		errWriteError = 0x1008,
		errAlreadyLoaded = 0x1009,
		errNotLoaded = 0x1010,
		errNotChanged = 0x1011,
		errFileError = 0x10000000,

		resSkipped = 1,
		resNothingToRead = 2,


	};
	typedef enError enResult;

#define DT_NOROW 0xFFFFFFFF


	enum enColumnType {
		ctypeInt		= 0,
		ctypeString		= 1,
		ctypeInt64		= 3,
		ctype64			= 3,
		ctypeBin		= 4, ///< Binary data stored with TypeBin
		ctypeDouble		= 6,
		ctypeUnknown	= -1,
	};

#define DT_CT_INT		::Stamina::DT::ctypeInt
#define DT_CT_PCHAR		::Stamina::DT::ctypeString
#define DT_CT_STR		DT_CT_PCHAR
#define DT_CT_STRING	::Stamina::DT::ctypeString
#define DT_CT_64		::Stamina::DT::ctypeInt64
#define DT_CT_BIN		::Stamina::DT::ctypeBin
#define DT_CT_UNKNOWN	::Stamina::DT::ctypeUnknown

#define DT_CT_CXOR		::Stamina::DT::cflagXor

#define DT_CT_TYPEMASK	::Stamina::DT::ctypeMask


	enum enColumnFlag {
		ctypeMask		= 0xFF,  ///< Mask for column type
		cflagNone		= 0,
		cflagDontSave	= 0x100,
		cflagNoSave		= 0x100, ///< This column won't be saved
		cflagKey		= 0x200, ///< This column is a key @deprecated
		cflagSecret		= 0x400, ///< This column contains secret personal information (like passwords)

		cflagIsLoaded   = 0x100000, ///< This column was loaded from file
		cflagIsDefined  = 0x200000, ///< This column was defined inside

		cflagCryptMask	= 0xFF000, ///< Mask for cryptographic flags
		cflagXor		= 0x01000, ///< This column uses XOR codecs
	};

#define DT_CF_NOSAVE   Stamina::DT::cflagNoSave
#define DT_CF_KEY      Stamina::DT::cflagKey
#define DT_CF_SECRET   Stamina::DT::cflagSecret

#define DT_CF_CXOR      Stamina::DT::cflagXor

	inline enColumnType operator | (const enColumnType & a, const enColumnFlag & b) {
		return (enColumnType) ((int)a | (int)b); 
	}
	inline enColumnFlag operator | (const enColumnFlag & a, const enColumnFlag & b) {
		return (enColumnFlag) ((int)a | (int)b); 
	}

	enum enRowFlag {
		rflagNone = 0,
		rflagDontSave = 1,
	};

// RowFlags
#define DT_RF_DONTSAVE  Stamina::DT::rflagDontSave

	enum GetSet {
		gsNone = 0,
		none = 0,
		getCopy = 0x10,
		getReference = 0,
		getHandler = 0x20,
		getToFile = 0x40,
		setHandler = 0x1000,
		setFromFile = 0x2000
	};

	inline GetSet operator | (const GetSet & a, const GetSet & b) {
		return (GetSet) ((int)a | (int)b); 
	}

	typedef GetSet GetFlags;
	typedef GetSet SetFlags;

	typedef unsigned int tColId;
	typedef enColumnType tColType;
	typedef unsigned int tRowId;
	typedef void* DataEntry;

	const tRowId rowNotFound = (tRowId) -1;
	const tRowId allRows = (tRowId) -1;
	const tColId colNotFound = (tRowId) -1;
	const tColId colByName = (tRowId) -1;
//	const tRowId rowIdFlag = DT_ROWID_MASK;



	class DTException: public Stamina::Exception {
	public:
		DTException(enError errorCode):errorCode(errorCode) {
		}
		enError errorCode;

		String getReason() {
			return "DTException";
		}
	};



} }



#endif