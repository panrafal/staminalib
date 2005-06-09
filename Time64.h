#ifndef __STAMINA_TIME64__
#define __STAMINA_TIME64__

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */




#include <time.h>

// Wrapper dla __time64_t
namespace Stamina {

	class Time64 {

	public:                 
		__time64_t sec;

		// Constructors
		Time64(struct tm * timer);
		Time64(time_t timer);
		Time64(bool current=false);
		Time64(__int64 timer);
		Time64(SYSTEMTIME &timer);
		Time64(class Date64 &timer);
		// Copy Contructors
		Time64 & operator=(__int64 & timer);
		Time64 & operator=(Date64 & timer);
		//  Time64 & operator=(__time64_t timer);
#ifdef _WINDOWS_
		Time64 & operator=( SYSTEMTIME & timer);
#endif
		// Cast operators
		operator __int64();
		operator tm();
#ifdef _WINDOWS_
		operator SYSTEMTIME();
#endif
		// Functions
		void strftime(char *strDest,size_t maxsize,const char *format);

#ifdef _STRING_
		std::string strftime(const char *format);
		/** Returns time string as 01:01 */
		std::string getTimeString(const char* hourStr = 0, const char* minStr = 0, const char* secStr = 0, bool needHour = false, bool needMin = false, bool needSec = false);
#endif
		bool empty();
		void clear();
		void now();

		int toDays();

	private:
		void from_time_t(time_t timer);
		void from_tm(struct tm * timer);
		void from_int64(__int64 timer);
		void from_date64(Date64 &timer);
		tm to_tm();
#ifdef _WINDOWS_
		void from_systemtime(SYSTEMTIME * st);
		SYSTEMTIME to_systemtime();
#endif
		friend class Date64;
	};


// Klasa z wydzielonymi czesciami czasu...

	class Date64 {
	public:                 // Maska do porownan to #FFFFFFFFFFFF
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

		//  int operator=(int n) {return 3;}
		//  bool operator > (sTime64 & t) {return 1;}
		Date64(struct tm * timer);
		Date64(time_t timer);
		Date64(bool current=false);
		Date64(__int64 timer);
		Date64(SYSTEMTIME &timer);
		//  Date64(__time64_t timer);
		Date64(Time64 & timer);
		// Copy Contructors
		Date64 & operator=( __int64 & timer);
#ifdef _WINDOWS_
		Date64 & operator=( SYSTEMTIME & timer);
#endif
		// Cast operators
		operator __int64() {
			return *(__int64*) this;
		}

		__time64_t getTime64() {
			Time64 t(*this);
			return t;
		}

		operator tm();
#ifdef _WINDOWS_
		operator SYSTEMTIME();
#endif
		// Functions
		void strftime(char *strDest,size_t maxsize,const char *format);
#ifdef _STRING_
		std::string strftime(const char *format);
#endif
		bool empty();
		void clear();
		void now();

	private:
		void from_time_t(time_t timer);
		void from_time64_t(__time64_t timer);
		void from_tm(struct tm * timer);
		void from_int64(__int64 timer);
		tm to_tm();
#ifdef _WINDOWS_
		void from_systemtime(SYSTEMTIME * st);
		SYSTEMTIME to_systemtime();
#endif
		friend class Time64;
	};


};
/*
  sTime64:
   0    4    8     12   16   20    24    28   32   36   40   44   48    52   56   60     
  |.... .... ..|.. ....|.... ..|.. ...|. ....|....|.... .... ....|...|. .... ....|.|xx|.|
     msec-10     sec-6   min-6  hour-5 day-5  m-4     year-12     wd3    yday9    d

*/


#endif
