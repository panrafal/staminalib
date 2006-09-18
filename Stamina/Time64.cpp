/*

The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License from
/LICENSE.HTML in this package or at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is "Stamina.lib" library code, released Feb 1, 2006.

The Initial Developer of the Original Code is "STAMINA" - Rafa³ Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa³ Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

*/

#include "stdafx.h"
#include <windows.h>
#include <string>
#include "Time64.h"


namespace Stamina {

	void Time64::from_date64(const Date64 &timer) {
		sec = timer.empty()?0 : _mktime64(&timer.to_tm());
	}

	// tm --


	void Time64::from_tm(const struct tm& timer) {
		sec = _mktime64((tm*) &timer);
	}

	tm Time64::to_tm() const {
		tm timer;
		memset(&timer , 0 , sizeof(tm));
#if (_MSC_VER >= 1400)
		_localtime64_s(&timer, &sec);
#else
		tm *tmp = _localtime64(&sec);
		if (tmp) {
			timer = *tmp;
		}
#endif
		return timer;
	}

	// time_t --

	void Time64::from_time_t(time_t timer) {
		sec = (__time64_t)timer;
	}

	// int64 --

	void Time64::from_int64(__int64 timer) {
		if (!(timer >> 63)) {
			sec = (__time64_t)timer;
		} else {
			Date64 cd(timer);
			from_date64(cd);
		}
	}

	// SYSTEMTIME --

	void Time64::from_systemtime(const SYSTEMTIME& st) {
		Date64 cd(st);
		from_date64(cd);
	}

	SYSTEMTIME Time64::to_systemtime() const {
		Date64 cd(*this);
		return cd.to_systemtime();
	}

	// FILETIME --

	void Time64::from_filetime(const FILETIME& st) {
		Date64 cd(st);
		from_date64(cd);
	}

	FILETIME Time64::to_filetime() const {
		Date64 cd(*this);
		return cd.to_filetime();
	}

	// ---------------------


	void Time64::strftime(char *strDest,size_t maxsize,const char *format) const {
		tm t = to_tm();
		::strftime(strDest , maxsize , format , &t);
	}

	std::string Time64::strftime(const char *format) const {
		char buff [501];
		strftime(buff , 500 , format);
		return buff;
	}


	std::string Time64::getTimeString(const char* hourStr, const char* minStr, const char* secStr, bool needHour, bool needMin, bool needSec) const {
		int hour = (int)this->sec / (60*60);
		int min = (int)(this->sec % (60*60)) / 60;
		int sec = (int)(this->sec % (60));
		std::string str;
		char buff [20];
		if (hour || needHour) {
#if (_MSC_VER >= 1400)
			sprintf_s(buff, 20, "%02d", hour);
#else
			sprintf(buff, "%02d", hour);
#endif
			str += buff;
			if (hourStr)
				str += hourStr;
			else
				str += ":";
		}
		if (min || (!hourStr && hour) || needMin) {
#if (_MSC_VER >= 1400)
			sprintf_s(buff, 20, "%02d", min);
#else
			sprintf(buff, "%02d", min);
#endif
			str += buff;
			if (minStr)
				str += minStr;
			else
				str += ":";

		}
		if (sec || (!hourStr && hour) || (!minStr && min) || needSec) {
#if (_MSC_VER >= 1400)
			sprintf_s(buff, 20, "%02d", sec);
#else
			sprintf(buff, "%02d", sec);
#endif
			str += buff;
			if (secStr)
				str += secStr;
		}
		return str;
	}


	// ----------------------------------------------------------
	// ----------------------------------------------------------




	void Date64::from_tm(const struct tm& timer) {
		msec = 0;
		sec = timer.tm_sec;
		min = timer.tm_min;
		hour = timer.tm_hour;
		day = timer.tm_mday;
		month = timer.tm_mon + 1;
		year = timer.tm_year + 1900;
		wday = timer.tm_wday;
		yday = timer.tm_yday + 1;
		isdst = timer.tm_isdst==1;
		desc=1;
		_align = 0;
	}

	tm Date64::to_tm() const {
		tm timer;
		timer.tm_sec = sec;
		timer.tm_min = min;
		timer.tm_hour = hour;
		timer.tm_mday = day;
		timer.tm_mon= month - 1;
		timer.tm_year = year - 1900;
		timer.tm_wday = wday;
		timer.tm_yday = yday - 1;
		timer.tm_isdst= isdst;
		return timer;
	}


	void Date64::from_time_t(time_t timer) {
#if (_MSC_VER >= 1400)
		tm t;
		localtime_s(&t, &timer);
		from_tm(t);
#else
		tm * t = localtime(&timer);
		from_tm(*t);
#endif
	}
	void Date64::from_time64_t(__time64_t timer) {
		if (!timer) {
			memcpy(this , &timer , 8);
			desc = 1;
		} else {
#if (_MSC_VER >= 1400)
			tm t;
			_localtime64_s(&t, &timer);
			from_tm(t);
#else
			tm *t = _localtime64(&timer);
			from_tm(*t);
#endif
		}
	}

	void Date64::from_int64(__int64 timer) {
		memcpy(this , &timer , 8);
		if (!desc) { // oops! to byl __time64_t
			from_time64_t((__time64_t)timer);	
		}
	}

	SYSTEMTIME Date64::to_systemtime() const {
		SYSTEMTIME st;
		st.wDay = this->day;
		st.wDayOfWeek = this->wday;
		st.wHour = this->hour;
		st.wMilliseconds = this->msec;
		st.wMinute = this->min;
		st.wMonth = this->month;
		st.wSecond = this->sec;
		st.wYear = this->year;
		return st;
	}

	void Date64::from_systemtime(const SYSTEMTIME& st) {
		this->desc = 1;
		this->day = st.wDay;
		this->hour = st.wHour;
		this->min = st.wMinute;
		this->month = st.wMonth;
		this->msec = st.wMilliseconds;
		this->sec = st.wSecond;
		this->wday = st.wDayOfWeek;
		this->year = st.wYear;
		this->_align = 0;
	}




	void Date64::strftime(char *strDest,size_t maxsize,const char *format) const {
		tm t = to_tm();
		::strftime(strDest , maxsize , format , &t);
	}

	std::string Date64::strftime(const char *format) const {
		char buff [501];
		strftime(buff , 500 , format);
		return buff;
	}



};
