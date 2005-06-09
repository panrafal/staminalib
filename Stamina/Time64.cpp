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
#include <time.h>
#include <math.h>
#include "Time64.h"


namespace Stamina {

Time64::Time64(struct tm * timer) {from_tm(timer);}
Time64::Time64(time_t timer) {from_time_t(timer);}
Time64::Time64(__int64 timer) {from_int64(timer);}
Time64::Time64(SYSTEMTIME &timer) {from_systemtime(&timer);}
Time64::Time64(class Date64 &timer) {from_date64(timer);}
//Time64::Time64(__time64_t timer) {sec = timer;}
// Copy Contructors
Time64 & Time64::operator=( __int64 & timer) {from_int64(timer);return *this;}
Time64 & Time64::operator=( SYSTEMTIME & timer) {from_systemtime(&timer);return *this;}
Time64 & Time64::operator=(Date64 & timer) {from_date64(timer); return *this;}
// Cast operators
Time64::operator tm() {return to_tm();}
Time64::operator SYSTEMTIME() {return to_systemtime();}

void Time64::from_date64(Date64 &timer) {
	sec = timer.empty()?0 : _mktime64(&timer.to_tm());
}


void Time64::from_tm(struct tm * timer) {
  if (!timer) return;
  sec = _mktime64(timer);
}

tm Time64::to_tm() {
  tm timer;
  memset(&timer , 0 , sizeof(tm));
  tm * tmp = _localtime64(&sec);
  if (tmp) {
	  timer = *tmp;
  }
  return timer;
}


void Time64::from_time_t(time_t timer) {
	sec = (__time64_t)timer;
}

void Time64::from_int64(__int64 timer) {
	if (!(timer >> 63)) {
		sec = (__time64_t)timer;
	} else {
		Date64 cd(timer);
		from_date64(cd);
	}
}


Time64::Time64(bool current) {
  if (current)
    sec = _time64(0);
  else
    sec = 0;
}

void Time64::now() {
    sec = _time64(0);
}

int Time64::toDays() {
	return floor((float)this->sec / (60*60*24));
}


Time64::operator __int64() {
  return (__int64)sec;
}

void Time64::strftime(char *strDest,size_t maxsize,const char *format){
  tm t = to_tm();
  ::strftime(strDest , maxsize , format , &t);
}

std::string Time64::strftime(const char *format) {
  char buff [501];
  strftime(buff , 500 , format);
  return buff;
}

void Time64::clear() {
  sec = 0;
}

bool Time64::empty() {
  return sec==0;
}

SYSTEMTIME Time64::to_systemtime() {
	Date64 cd(*this);
	return cd.to_systemtime();
}

void Time64::from_systemtime(SYSTEMTIME * st) {
	Date64 cd(*st);
	from_date64(cd);
}


std::string Time64::getTimeString(const char* hourStr, const char* minStr, const char* secStr, bool needHour, bool needMin, bool needSec) {
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


Date64::Date64(struct tm * timer) {from_tm(timer);}
Date64::Date64(time_t timer) {from_time_t(timer);}
Date64::Date64(__int64 timer) {from_int64(timer);}
Date64::Date64(SYSTEMTIME &timer) {from_systemtime(&timer);}
Date64::Date64(Time64 &timer) {from_time64_t(timer.sec);}
//Date64::Date64(__time64_t timer) {from_time64_t(timer);}

// Copy Contructors
Date64 & Date64::operator=( __int64 & timer) {from_int64(timer);return *this;}
Date64 & Date64::operator=( SYSTEMTIME & timer) {from_systemtime(&timer);return *this;}
// Cast operators
Date64::operator tm() {return to_tm();}
Date64::operator SYSTEMTIME() {return to_systemtime();}

void Date64::from_tm(struct tm * timer) {
  if (!timer) return;
  msec = 0;
  sec = timer->tm_sec;
  min = timer->tm_min;
  hour = timer->tm_hour;
  day = timer->tm_mday;
  month = timer->tm_mon + 1;
  year = timer->tm_year + 1900;
  wday = timer->tm_wday;
  yday = timer->tm_yday + 1;
  isdst = timer->tm_isdst==1;
  desc=1;
  _align = 0;
}

tm Date64::to_tm() {
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
  from_tm(localtime(&timer));
}
void Date64::from_time64_t(__time64_t timer) {
	if (!timer) {
	  memcpy(this , &timer , 8);
	  desc = 1;
	} else
      from_tm(_localtime64(&timer));
}

void Date64::from_int64(__int64 timer) {
  memcpy(this , &timer , 8);
  if (!desc) { // oops! to byl __time64_t
	from_time64_t((__time64_t)timer);	
  }
}


Date64::Date64(bool current) {
  if (current)
    from_time_t(time(0));
  else
    from_int64(0);
}

void Date64::now() {
  from_time_t(time(0));
}

void Date64::strftime(char *strDest,size_t maxsize,const char *format){
  tm t = to_tm();
  ::strftime(strDest , maxsize , format , &t);
}

std::string Date64::strftime(const char *format) {
  char buff [501];
  strftime(buff , 500 , format);
  return buff;
}

void Date64::clear() {
  from_int64(0);
}

bool Date64::empty() {
  __int64 temp;
  memcpy(&temp , this , 8);
  return (temp & 0x7FFFFFFFFFFFFFFF)==0;
}

SYSTEMTIME Date64::to_systemtime() {
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

void Date64::from_systemtime(SYSTEMTIME * st) {
    this->desc = 1;
    this->day = st->wDay;
    this->hour = st->wHour;
    this->min = st->wMinute;
    this->month = st->wMonth;
    this->msec = st->wMilliseconds;
    this->sec = st->wSecond;
    this->wday = st->wDayOfWeek;
    this->year = st->wYear;
	this->_align = 0;
}

};
