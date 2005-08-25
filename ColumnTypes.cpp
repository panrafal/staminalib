#include "stdafx.h"
#include "ColumnTypes.h"


namespace Stamina { namespace DT {

	typedef Column_template<ColumnType_int> CT_int;

	template <>
	void CT_int::setTypeData(iRow* row, CT_int::tSetTypeData val) const {
		this->setData(row, val);
	}

	template <>
	CT_int::tGetTypeData CT_int::getTypeData(const iRow* row, bool useDefault) const {
		return (int)this->getData(row, useDefault);
	}


} }