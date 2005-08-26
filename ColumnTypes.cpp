#include "stdafx.h"

#include "../Helpers.h"

#include "ColumnTypes.h"


namespace Stamina { namespace DT {


	// ------------------------  CONVERSION

	typedef ColumnType_bin::tConvert tBinConvert;

	// --- from INT

	template<> extern inline int convert<int, int>(int v) {
		return v;
	}

	template<> extern inline __int64 convert<__int64, int>(int v) {
		return v;
	}

	template<> extern inline  double convert<double, int>(int v) {
		return v;
	}

	template<> extern inline  PassStringRef convert<PassStringRef, int>(int v) {
		String s;
		_itoa(v, s.useBuffer<char>(10), 10);
		s.releaseBuffer<char>();
		return s;
	}

	template<> extern inline  tBinConvert convert<tBinConvert, int>(int v) {
		return ByteBuffer();
	}


	// --- from INT64

	template<> extern inline int convert<int, __int64>(__int64 v) {
		return (int)v;
	}

	template<> extern inline __int64 convert<__int64, __int64>(__int64 v) {
		return v;
	}

	template<> extern inline  double convert<double, __int64>(__int64 v) {
		return (double)v;
	}

	template<> extern inline  PassStringRef convert<PassStringRef, __int64>(__int64 v) {
		String s;
		_i64toa(v, s.useBuffer<char>(20), 10);
		s.releaseBuffer<char>();
		return s;
	}

	template<> extern inline  tBinConvert convert<tBinConvert, __int64>(__int64 v) {
		return ByteBuffer();
	}


	// --- from DOUBLE

	template<> extern inline int convert<int, double>(double v) {
		return (int)v;
	}

	template<> extern inline __int64 convert<__int64, double>(double v) {
		return (__int64)v;
	}

	template<> extern inline  double convert<double, double>(double v) {
		return v;
	}

	template<> extern inline  PassStringRef convert<PassStringRef, double>(double v) {
		String s;
		gcvt(v, 20, s.useBuffer<char>(20));
		s.releaseBuffer<char>();
		return s;
	}

	template<> extern inline  tBinConvert convert<tBinConvert, double>(double v) {
		return ByteBuffer();
	}


	// --- from STRING

	template<> extern inline int convert<int, const StringRef&>(const StringRef& v) {
		return chtoint(v.a_str());
	}

	template<> extern inline __int64 convert<__int64, const StringRef&>(const StringRef& v) {
		return chtoint64(v.a_str());
	}

	template<> extern inline  double convert<double, const StringRef&>(const StringRef& v) {
		return strtod(v.a_str(), 0);
	}

	template<> extern inline  PassStringRef convert<PassStringRef, const StringRef&>(const StringRef& v) {
		return StringRef(v);
	}

	template<> extern inline  tBinConvert convert<tBinConvert, const StringRef&>(const StringRef& v) {
		v.prepareType<char>();
		return ByteBuffer((unsigned char*)v.str<char>(), v.getDataSize<char>());
	}


	// --- from BIN

	template<> extern inline int convert<int, const ByteBuffer&>(const ByteBuffer& v) {
		return 0;
	}

	template<> extern inline __int64 convert<__int64, const ByteBuffer&>(const ByteBuffer& v) {
		return 0;
	}

	template<> extern inline  double convert<double, const ByteBuffer&>(const ByteBuffer& v) {
		return 0;
	}

	template<> extern inline  PassStringRef convert<PassStringRef, const ByteBuffer&>(const ByteBuffer& v) {
		StringRef s((const char*)v.getBuffer(), v.getLength());
		return s;
	}

	template<> extern inline  tBinConvert convert<tBinConvert, const ByteBuffer&>(const ByteBuffer& v) {
		return ByteBuffer::BufferRef( v );
	}






	// ------------------------  TYPES

	// --- int

	typedef Column_template<ColumnType_int> CT_int;

	template <>
	void CT_int::setTypeData(iRow* row, CT_int::tSetTypeData val) const {
		this->setData(row, val);
	}

	template <>
	CT_int::tGetTypeData CT_int::getTypeData(const iRow* row, bool useDefault) const {
		return (tData)this->getData(row, useDefault);
	}

	// --- int64

	typedef Column_template<ColumnType_int64> CT_int64;

	template <>
	void CT_int64::setTypeData(iRow* row, CT_int64::tSetTypeData val) const {
		tData data = (tData)this->getData(row, false);
		if (data == 0) {
			data = new tType;
			this->setData(row, data);
		}
		*data = val;
	}

	template <>
	CT_int64::tGetTypeData CT_int64::getTypeData(const iRow* row, bool useDefault) const {
		tData data = (tData)this->getData(row, useDefault);
		if (data) {
			return *data;
		} else {
			return 0;
		}
	}

	// --- double

	typedef Column_template<ColumnType_double> CT_double;

	template <>
	void CT_double::setTypeData(iRow* row, CT_double::tSetTypeData val) const {
		tData data = (tData)this->getData(row, false);
		if (data == 0) {
			data = new tType;
			this->setData(row, data);
		}
		*data = val;
	}

	template <>
	CT_double::tGetTypeData CT_double::getTypeData(const iRow* row, bool useDefault) const {
		tData data = (tData)this->getData(row, useDefault);
		if (data) {
			return *data;
		} else {
			return 0;
		}
	}


	// --- string

	typedef Column_template<ColumnType_string> CT_string;

	template <>
	void CT_string::setTypeData(iRow* row, CT_string::tSetTypeData val) const {
		tData data = (tData)this->getData(row, false);
		if (data == 0) {
			data = new tType;
			this->setData(row, data);
		}
		*data = val;
	}

	template <>
	CT_string::tGetTypeData CT_string::getTypeData(const iRow* row, bool useDefault) const {
		tData data = (tData)this->getData(row, useDefault);
		if (data) {
			return *data;
		} else {
			return "";
		}
	}

	// --- bin

	typedef Column_template<ColumnType_bin> CT_bin;

	template <>
	void CT_bin::setTypeData(iRow* row, CT_bin::tSetTypeData val) const {
		tData data = (tData)this->getData(row, false);
		if (data == 0) {
			data = new tType;
			this->setData(row, data);
		}
		data->assign(val.getBuffer(), val.getLength());
	}

	template <>
	CT_bin::tGetTypeData CT_bin::getTypeData(const iRow* row, bool useDefault) const {
		tData data = (tData)this->getData(row, useDefault);
		if (data) {
			return *data;
		} else {
			return ByteBuffer();
		}
	}

} }