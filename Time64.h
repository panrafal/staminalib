#ifndef __STAMINA_TIME64__
#define __STAMINA_TIME64__

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */




#include <time.h>
#include <math.h>

// Wrapper dla __time64_t
namespace Stamina {

	class Date64;

	class Time64 {

	public:                 
		__time64_t sec;

	public:

		// Constructors
		Time64(bool current = false) {
			if (current)
				sec = _time64(0);
			else
				sec = 0;
		}
		inline Time64(const struct tm& timer) {
			from_tm(timer);
		}
		inline Time64(time_t timer) {
			from_time_t(timer);
		}
		inline Time64(__int64 timer) {
			from_int64(timer);
		}
#ifdef _WINDOWS_
		inline Time64(const SYSTEMTIME &timer) {
			from_systemtime(timer);
		}
		inline Time64(const FILETIME &timer) {
			from_filetime(timer);
		}
#endif
		inline Time64(const Date64& timer) {
			from_date64(timer);
		}

		// Copy Contructors
		inline Time64 & operator=( const __int64 & timer) {
			from_int64(timer);
			return *this;
		}
#ifdef _WINDOWS_
		inline Time64 & operator=( const SYSTEMTIME & timer) {
			from_systemtime(timer);
			return *this;
		}
		inline Time64 & operator=( const FILETIME & timer) {
			from_filetime(timer);
			return *this;
		}
#endif
		inline Time64 & operator=(const Date64 & timer) {
			from_date64(timer); 
			return *this;
		}

		// Cast operators
		inline operator __int64() const {
			return (__int64)sec;
		}
		inline operator tm() const {
			return to_tm();
		}
#ifdef _WINDOWS_
		inline operator SYSTEMTIME() const {
			return to_systemtime();
		}
		inline operator FILETIME() const {
			return to_filetime();
		}
#endif

		//operator Date64() const;

		// Compare

		inline bool operator == (const Time64& b) const {
			return this->sec == b.sec;
		}
		inline bool operator != (const Time64& b) const {
			return this->sec != b.sec;
		}
		inline bool operator < (const Time64& b) const {
			return this->sec < b.sec;
		}
		inline bool operator > (const Time64& b) const {
			return this->sec > b.sec;
		}
		inline bool operator <= (const Time64& b) const {
			return this->sec <= b.sec;
		}
		inline bool operator >= (const Time64& b) const {
			return this->sec >= b.sec;
		}

		// arithmetic

		inline Time64 operator - (const Time64& b) const {
			return Time64( this->sec - b.sec );
		}
		inline Time64 operator + (const Time64& b) const {
			return Time64( this->sec + b.sec );
		}

		// Functions
		void strftime(char *strDest,size_t maxsize,const char *format) const;

#ifdef _STRING_
		std::string strftime(const char *format) const;
		/** Returns time string as 01:01 */
		std::string getTimeString(const char* hourStr = 0, const char* minStr = 0, const char* secStr = 0, bool needHour = false, bool needMin = false, bool needSec = false) const;
#endif
		inline void clear() {
			sec = 0;
		}

		inline bool empty() const {
			return sec == 0;
		}

		void now() {
			sec = _time64(0);
		}

		int toDays() const {
			return (int)floor((float)this->sec / (60*60*24));
		}

	private:
		void from_time_t(time_t timer);
		void from_tm(const struct tm& timer);
		void from_int64(__int64 timer);
		void from_date64(const Date64& timer);
		tm to_tm() const;
#ifdef _WINDOWS_
		void from_systemtime(const SYSTEMTIME& st);
		SYSTEMTIME to_systemtime() const;
		void from_filetime(const FILETIME& st);
		FILETIME to_filetime() const;
#endif
		friend class Date64;
	};


// Klasa z wydzielonymi czesciami czasu...

	class Date64 {
	public:                 // Maska do porownan to #FFFFFFFFFFFF

		const static __int64 compareMask = 0xFFFFFFFFFFFF;

		unsigned msec  : 10;   // 0..999                        0x3FF
		unsigned sec   : 6;    // 0..59                        0xFC00
		unsigned min   : 6;    // 0..59                      0x3F0000
		unsigned hour  : 5;    // 0..23                       7C00000
		unsigned day   : 5;    // 1..31                      F8000000
		unsigned month : 4;    // 1..12                     F00000000
		unsigned year  : 12;   // 0..4095                FFF000000000
		unsigned wday  : 3;    // 0..7 (1/7 - sunday    7000000000000
		unsigned yday  : 9;    // 1..366              FF8000000000000
		unsigned isdst : 1;    // 0.1
		unsigned _align: 2;
		unsigned desc  : 1;    // Zawsze = 1

	public:
		//  int operator=(int n) {return 3;}
		//  bool operator > (sTime64 & t) {return 1;}

		Date64(bool current = false) {
			if (current)
				from_time_t(time(0));
			else
				from_int64(0);
		}
		Date64(const struct tm& timer) {
			from_tm(timer);
		}
		Date64(time_t timer) {
			from_time_t(timer);
		}
		Date64(__int64 timer) {
			from_int64(timer);
		}
#ifdef _WINDOWS_
		Date64(const SYSTEMTIME& timer) {
			from_systemtime(timer);
		}
		Date64(const FILETIME& timer) {
			from_filetime(timer);
		}
#endif
		Date64(const Time64& timer) {
			from_time64_t(timer.sec);
		}

		// Copy Contructors
		Date64 & operator=( const __int64 & timer) {
			from_int64(timer);
			return *this;
		}
#ifdef _WINDOWS_
		Date64 & operator=( const SYSTEMTIME & timer) {
			from_systemtime(timer);
			return *this;
		}
		Date64 & operator=( const FILETIME & timer) {
			from_filetime(timer);
			return *this;
		}
#endif
		Date64& operator=(const Time64& timer) {
			from_time64_t(timer.sec);
			return *this;
		}

		// Cast operators
		operator tm() const {
			return to_tm();
		}

#ifdef _WINDOWS_
		operator SYSTEMTIME() const {
			return to_systemtime();
		}
		operator FILETIME() const {
			return to_filetime();
		}
#endif

/*		inline operator Time64() const {
			return Time64(*this);
		}*/

		//operator __int64() const {
		//	return *(__int64*) this;
		//}

		__time64_t getTime64() const {
			Time64 t(*this);
			return t;
		}


		// Compare

		inline bool operator == (const Date64& b) const {
			return this->getCmpInt() == b.getCmpInt();
		}
		inline bool operator != (const Date64& b) const {
			return this->getCmpInt() != b.getCmpInt();
		}
		inline bool operator < (const Date64& b) const {
			return this->getCmpInt() < b.getCmpInt();
		}
		inline bool operator > (const Date64& b) const {
			return this->getCmpInt() > b.getCmpInt();
		}
		inline bool operator <= (const Date64& b) const {
			return this->getCmpInt() <= b.getCmpInt();
		}
		inline bool operator >= (const Date64& b) const {
			return this->getCmpInt() >= b.getCmpInt();
		}


		// Functions
		void strftime(char *strDest,size_t maxsize,const char *format) const;
#ifdef _STRING_
		std::string strftime(const char *format) const;
#endif

		void clear() {
			from_int64(0);
		}

		bool empty() const {
			__int64 temp;
			memcpy(&temp , this , 8);
			return (temp & 0x7FFFFFFFFFFFFFFF)==0;
		}
		void now() {
			from_time_t(time(0));
		}

		inline __int64 getInt64() const {
			return *(__int64*) this;
		}
		/** Returns date as an comparable integer */
		inline unsigned __int64 getCmpInt() const {
			return (*(__int64*) this) & compareMask;
		}

	private:
		void from_time_t(time_t timer);
		void from_time64_t(__time64_t timer);
		void from_tm(const struct tm& timer);
		void from_int64(__int64 timer);
		tm to_tm() const;
#ifdef _WINDOWS_
		void from_systemtime(const SYSTEMTIME& st);
		SYSTEMTIME to_systemtime() const;
		void from_filetime(const FILETIME& ft) {
			SYSTEMTIME st;
			FileTimeToSystemTime(&ft, &st);
			from_systemtime(st);
		}
		FILETIME to_filetime() const {
			FILETIME ft;
			SYSTEMTIME st = this->to_systemtime();
			SystemTimeToFileTime(&st, &ft);
			return ft;
		}
#endif
		friend class Time64;
	};



/*	inline Time64::operator Date64() const {
		return Date64(*this);
	}
*/

};
/*
  sTime64:
   0    4    8     12   16   20    24    28   32   36   40   44   48    52   56   60     
  |.... .... ..|.. ....|.... ..|.. ...|. ....|....|.... .... ....|...|. .... ....|.|xx|.|
     msec-10     sec-6   min-6  hour-5 day-5  m-4     year-12     wd3    yday9    d

*/


#endif
