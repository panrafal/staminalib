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
/*
	inline Value ValueStr(const char* value, int buffSize=0) {
		Value v(ctypeString);
		v.vCChar = value;
		v.buffSize = buffSize;
		return v;
	}
	inline Value ValueStrDuplicate() {
		Value v(ctypeString);
		v.vChar = 0;
		v.buffSize = -1;
		return v;
	}
	inline Value ValueStrGetSize() {
		Value v(ctypeString);
		v.vChar = 0;
		v.buffSize = 0;
		return v;
	}

	inline Value ValueWideStr(const wchar_t* value, int buffSize=0) {
		Value v(ctypeWideString);
		v.vCWChar = value;
		v.buffSize = buffSize;
		return v;
	}
	inline Value ValueWideStrDuplicate() {
		Value v(ctypeWideString);
		v.vWChar = 0;
		v.buffSize = -1;
		return v;
	}
	inline Value ValueWideStrGetSize() {
		Value v(ctypeWideString);
		v.vChar = 0;
		v.buffSize = 0;
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

*/




} }



#endif