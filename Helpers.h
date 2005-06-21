/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

namespace Stamina {

	inline int round(double val) {
		return (int)floor(val + 0.5);
	};

	inline void randomSeed() {
#ifdef _WINDOWS_
		srand(GetTickCount());
#else
		srand(time(0));
#endif
	}

	unsigned int random(unsigned int min = 0, unsigned int max = RAND_MAX);

	const char * inttoch(int v , char * s , int radix=10 , int max=-1 , bool upper=true);

	char * _vsaprintf(const char *format, va_list ap);
	char * _saprintf(const char *format, ...);

#ifdef _STRING_
	std::string inttostr(int v , int radix=10 , int max=-1 , bool upper=true);

	inline std::string stringf(const char *format, ...) {
		va_list ap;

		va_start(ap, format);
		char * msg = _vsaprintf(format, ap);
			std::string _msg = msg;
			free(msg);
			va_end(ap);
			return _msg;
	}
#endif
	int chtoint(const char * str , unsigned char base=0xFF);
	__int64 chtoint64(const char * str , unsigned char base = 0xFF);



	inline unsigned char chval(unsigned char ch) { // zamienia znak na liczbe i odwrotnie
		/* Zamieniamy liczbe na znak */
		if (ch < 10) return '0'+ch; // 0 - 10
		if (ch < '0') return 'A'+(ch-10); // A - ...
		/* Zamieniamy znak na liczbe */
		if (ch < 'A') return ch-'0'; // 0-10
		if (ch < 'a') return ch-'A'+10; // A-...
		return ch-'a'+10; // a-...
	}

	char * str_tr(char * str , const char * chIn , const char * chOut);

#ifdef _STRING_

	template <class STR>
		inline typename STR::value_type* stringBuffer(STR& str, typename STR::size_type size = STR::npos) 
	{
		if (size != STR::npos) {
			str.resize(size+1);
		}
		STR::value_type* ch = (STR::value_type*) str.c_str();
		ch[str.length()-1] = 0;
		return ch;
	}

	template <class STR>
		inline void stringRelease(STR& str, typename STR::size_type size = STR::npos) 
	{
		str.resize(size == STR::npos ? strlen(str.c_str()) : size);
	}

	std::string urlEncode(const std::string& str , char special = '%' , char * noChange = 0);
	std::string urlDecode(const std::string& str , char special = '%');


#ifdef _DEQUE_

	typedef std::deque<std::string> tStringVector;

	void split(const std::string & txt, const std::string & splitter, tStringVector & list, bool all = true);

	void splitCommand(const std::string& txt , char splitter ,  tStringVector & list);

#endif

#endif

	char * stripSlashes(char * str);

	/**Reverses buffer contents*/
	void * memrev(void * buff , int count);

	int ipToLong(const char * ip);
#ifdef _STRING_
	std::string longToIp(long adr);
#endif


// Directories

#ifdef _STRING_
	/**Removes directory with all contents.
	@return number of deleted files/dirs
	*/
	int removeDirTree(const std::string& path);
	/**Creates all missing directories on provided path.
	@return number of created directories
	*/
	int createDirectories(const std::string& path);
#endif


};