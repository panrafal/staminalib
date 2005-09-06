#pragma once
/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: DT.h 93 2005-08-22 00:23:52Z hao $
 */

/**@file All basic definitions needed to work with DataTable

*/

#include "DT.h"
#include <Stamina\ObjectImpl.h>
#include <Stamina\Lock.h>

namespace Stamina { namespace DT {


	class Value: public SharedObject<iSharedObject, LockableObject<iSharedObject, Lock_blank > > {
	public:

		virtual enColumnType getType() const = 0;

	};

	class Value_int: public Value {
	public:

		STAMINA_OBJECT_CLASS(DT::Value_int, Value);

		Value_int(int v) {
			_v = v;
		}

		virtual enColumnType getType() const {
			return ctypeInt;
		}
		int getInt() const {
			return _v;
		}
		int& operator * () {
			return _v;
		}
		int cmp(int b) const {
			if (_v < b) return -1;
			if (_v > b) return 1;
			return 0;
		}
		bool equals(int b) const {
			return this->cmp(b) == 0;
		}
	private:
		int _v;
	};

	class Value_int64: public Value {
	public:

		STAMINA_OBJECT_CLASS(DT::Value_int64, Value);

		Value_int64(__int64 v) {
			_v = v;
		}

		virtual enColumnType getType() const {
			return ctypeInt64;
		}
		__int64 getInt64() const {
			return _v;
		}
		int cmp(__int64 b) const {
			if (_v < b) return -1;
			if (_v > b) return 1;
			return 0;
		}
		bool equals(__int64 b) const {
			return this->cmp(b) == 0;
		}
	private:
		__int64 _v;
	};

	class Value_double: public Value {
	public:

		STAMINA_OBJECT_CLASS(DT::Value_double, Value);

		Value_double(double v) {
			_v = v;
		}

		virtual enColumnType getType() const {
			return ctypeDouble;
		}
		double getDouble() const {
			return _v;
		}
		int cmp(double b) const {
			if (_v < b) return -1;
			if (_v > b) return 1;
			return 0;
		}
		bool equals(double b) const {
			return this->cmp(b) == 0;
		}
	private:
		double _v;
	};


	class Value_string: public Value {
	public:

		STAMINA_OBJECT_CLASS(DT::Value_string, Value);

		Value_string(const StringRef& v) {
			_v.assign(v);
		}

		virtual enColumnType getType() const {
			return ctypeString;
		}
		const String& getString() const {
			return _v;
		}
		int cmp(const StringRef& b) const {
			return _v.compare(b, true);
		}
		bool equals(const StringRef& b) const {
			return _v.equal(b, true);
		}
	protected:

		Value_string() {
		}

		String _v;
	};


	class Value_stringRef: public Value_string {
	public:

		STAMINA_OBJECT_CLASS(DT::Value_stringRef, Value_string);

		Value_stringRef(const StringRef& v):Value_string() {
			_v.assignCheapReference(v);
		}

	};


	class Value_bin: public Value {
	public:

		STAMINA_OBJECT_CLASS(DT::Value_bin, Value);

		Value_bin(const ByteBuffer& v) {
			_v.assign(v.getBuffer(), v.getLength());
		}

		virtual enColumnType getType() const {
			return ctypeBin;
		}
		const ByteBuffer& getBin() const {
			return _v;
		}
		int cmp(const ByteBuffer& b) const {
			int r;
			int l = min((_v.getLength()), (b.getLength()));

			r = memcmp(_v.getString(), b.getString(), l);
			if (r == 0) {
				if ((_v.getLength()) > (b.getLength())) 
					return 1;
				else if ((_v.getLength()) < (b.getLength())) 
					return -1;
			}
			return r;
		}
		bool equals(const ByteBuffer& b) const {
			return this->cmp(b) == 0;
		}
	protected:
		Value_bin() {
		}

		ByteBuffer _v;
	};

	class Value_binRef: public Value_bin {
	public:

		STAMINA_OBJECT_CLASS(DT::Value_binRef, Value_bin);

		Value_binRef(const ByteBuffer& v):Value_bin() {
			_v.assignCheapReference(v.getBuffer(), v.getLength());
		}

	};



	typedef SharedPtr<Value> oValue;


} }