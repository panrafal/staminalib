#pragma once
#ifndef __DT_DTDEPRECATED__
#define __DT_DTDEPRECATED__

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id:  $
 */

/**@file All basic definitions needed to work with DataTable

*/

namespace Stamina { namespace DT {



	struct _OldTypeBin {
		void * buff;
		unsigned int size;
	};

	class OldTypeBin: public _OldTypeBin {
	public:
		OldTypeBin(void* buff = 0, unsigned int size = 0) {
			this->buff = buff;
			this->size = size;
		}

		OldTypeBin(const _OldTypeBin& b) {
			this->buff = b.buff;
			this->size = b.size;
		}

		bool operator == (const OldTypeBin& b) {
			if (size != b.size) return false;
			return memcmp(buff, b.buff, size) == 0;
		}
	};

	#pragma pack(push, 1)
	struct OldValue {
		short type; ///< Typ przekazywanej wartoœci
		union {
			struct {
				union {
					char * vChar;
					const char * vCChar;
					wchar_t * vWChar;
					const wchar_t * vCWChar;
				};
				int buffSize;
			};
			int vInt;
			__int64 vInt64;
			_OldTypeBin vBin;

		};
		OldValue(tColType type=ctypeUnknown):type((short)type) {vInt64 = 0;buffSize=0;}

		enColumnType getType() const {
			return (enColumnType) this->type;
		}
	};
	#pragma pack(pop)

	inline OldValue OldValueStr(const char* value, int buffSize=0) {
		OldValue v(ctypeString);
		v.vCChar = value;
		v.buffSize = buffSize;
		return v;
	}
	inline OldValue OldValueStrDuplicate() {
		OldValue v(ctypeString);
		v.vChar = 0;
		v.buffSize = -1;
		return v;
	}

	inline OldValue OldValueInt(int value) {
		OldValue v(ctypeInt);
		v.vInt = value;
		return v;
	}
	inline OldValue OldValueInt64(__int64 value) {
		OldValue v(ctypeInt64);
		v.vInt64 = value;
		return v;
	}
	inline OldValue OldValueBin(const OldTypeBin& val) {
		OldValue v(ctypeBin);
		v.vBin = val;
		return v;
	}
	inline OldValue OldValueBin(void* data, int size) {
		OldValue v(ctypeBin);
		v.vBin.buff = data;
		v.vBin.size = size;
		return v;
	}




	class OldFind {
	public:

		enum Operation {
			eq, neq, gt, gteq, lt, lteq
		};

		inline OldFind(Operation operation, tColId col, const OldValue& value):operation(operation), col(col), value(value) {}

		static inline OldFind EqStr(tColId col, const char* str) {
			return OldFind(eq, col, OldValueStr(str));
		}
#ifdef _STRING_
		static inline OldFind EqStr(tColId col, const std::string& str) {
			return OldFind(eq, col, OldValueStr(str.c_str()));
		}
#endif
		static inline OldFind EqInt(tColId col, int value) {
			return OldFind(eq, col, OldValueInt(value));
		}

		Operation operation;
		tColId col;
		OldValue value;
	};


} }



#endif