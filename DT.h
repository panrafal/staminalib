/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id $
 */

/**@file All basic definitions needed to work with DataTable

*/

namespace Stamina { namespace DT {

#define DT_READ 1
#define DT_WRITE 2
#define DT_APPEND 4

	const unsigned int rowIdFlag = 0x40000000;
	const unsigned int rowIdMax = 0x3FFFFFFF;
	const unsigned int rowIdMin = 0x1;

	const unsigned int colIdUniqueFlag = 0x00800000;

#define DT_CC_RESIZE 1
#define DT_CC_FILL 2

// RowFlags
#define DT_RF_DONTSAVE  0x1

#define DT_FT_TEXT 0
#define DT_FT_BINARY 1
//#define DT_FT_AUTO -1

	enum enError {
		errNoColumn = 100,
		errNoRow = 200,
		errBadType = 300,
	};

#define DT_NOROW 0xFFFFFFFF


	enum enColumnType {
		ctypeInt		= 0,
		ctypeString		= 1,
		ctypeInt64		= 3,
		ctype64			= 3,
		ctypeBin		= 4, ///< Binary data stored with TypeBin
		ctypeUnknown	= -1,
	};

#define DT_CT_INT		Stamina::DT::ctypeInt
#define DT_CT_PCHAR		Stamina::DT::ctypeString
#define DT_CT_STR		DT_CT_PCHAR
#define DT_CT_STRING	Stamina::DT::ctypeString
#define DT_CT_64		Stamina::DT::ctypeInt64
#define DT_CT_BIN		Stamina::DT::ctypeBin
#define DT_CT_UNKNOWN	Stamina::DT::ctypeUnknown

#define DT_CT_TYPEMASK	Stamina::DT::ctypeMask


	enum enColumnFlag {
		ctypeMask		= 0xFF,  ///< Mask for column type
		cflagDontSave	= 0x100,
		cflagNoSave		= 0x100, ///< This column won't be saved
		cflagKey		= 0x200, ///< This column is a key @deprecated
		cflagSecret		= 0x400, ///< This column contains secret personal information (like passwords)

		cflagIsLoaded   = 0x100000, ///< This column was loaded from file

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

	typedef unsigned int tColId;
	typedef enColumnType tColType;
	typedef unsigned int tRowId;

	const tRowId rowNotFound = -1;
	const tRowId allRows = -1;
	const tColId colNotFound = -1;
	const tColId colByName = -1;
	const tRowId rowIdMask = DT_ROWID_MASK;

	struct TypeBin {
		int size;
		void * buff;
	};

	#pragma pack(push, 1)
	struct Value {
		short type; ///< Typ przekazywanej wartoœci
		union {
			struct {
				union {
					char * vChar;
					const char * vCChar;
				};
				unsigned int buffSize;
			};
			int vInt;
			__int64 vInt64;
			TypeBin vBin;

		};
		Value(tColType type=ctypeUnknown):type(type) {vInt64 = 0;buffSize=0;}

	};
	#pragma pack(pop)

	inline Value ValueStr(const char* value, int buffSize=0) {
		Value v(ctypeString);
		v.vCChar = value;
		v.buffSize = buffSize;
		return v;
	}
	inline Value ValueInt(int value) {
		Value v(ctypeInt);
		v.vInt = value;
		return v;
	}
	inline Value ValueInt64(__int64 value) {
		Value v(ctypeInt64);
		v.vInt64 = value;
		return v;
	}
	inline Value ValueBin(const TypeBin& val) {
		Value v(ctypeBin);
		v.vBin = val;
		return v;
	}
	inline Value ValueBin(void* data, int size) {
		Value v(ctypeBin);
		v.vBin.buff = data;
		v.vBin.size = size;
		return v;
	}

	class Find {
	public:

		enum Operation {
			eq, neq, gt, gteq, lt, lteq
		};

		inline Find(Operation operation, tColId col, const Value& value):operation(operation), col(col), value(value) {}

		static inline Find EqStr(tColId col, const char* str) {
			return Find(eq, col, ValueStr(str));
		}
		static inline Find EqInt(tColId col, int value) {
			return Find(eq, col, ValueInt(value));
		}

		Operation operation;
		tColId col;
		Value value;
	};





} }