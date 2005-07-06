/*
*  Stamina.LIB
*  
*  Please READ /License.txt FIRST! 
* 
*  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
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
		tm * tmp = _localtime64(&sec);
		if (tmp) {
			timer = *tmp;
		}
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
		int hour = this->sec / (60*60);
		int min = (this->sec % (60*60)) / 60;
		int sec = (this->sec % (60));
		std::string str;
		char buff [20];
		if (hour || needHour) {
			sprintf(buff, "%02d", hour);
			str += buff;
			if (hourStr)
				str += hourStr;
			else
				str += ":";
		}
		if (min || (!hourStr && hour) || needMin) {
			sprintf(buff, "%02d", min);
			str += buff;
			if (minStr)
				str += minStr;
			else
				str += ":";

		}
		if (sec || (!hourStr && hour) || (!minStr && min) || needSec) {
			sprintf(buff, "%02d", sec);
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
		from_tm(*localtime(&timer));
	}
	void Date64::from_time64_t(__time64_t timer) {
		if (!timer) {
			memcpy(this , &timer , 8);
			desc = 1;
		} else
			from_tm(*_localtime64(&timer));
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
