/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#include "stdafx.h"
#include <deque>
#include <direct.h>
#include <io.h>
#include <stdstring.h>
#include "Helpers.h"


using namespace std;

namespace Stamina {

	struct __initializer {
		__initializer() {
			srand(time(0));
		}
	} _initializer;


	unsigned int random(unsigned int min, unsigned int max) {
		static bool seeded = false;
		if (!seeded) {
			randomSeed();
			seeded = true;
		}
		unsigned int r = rand();
		unsigned int width = RAND_MAX;
		if (max - min > width) {
			width = 0x3FFFFFFF;
			r = r | (rand() << 15);
			if (max - min > width) {
				width = 0xFFFFFFFF;
				r = r | (rand() << 30);
			}
		}
		r = round(r * (double(max - min) / double(width)));
		return r + min;
	}


	const char * inttoch(int v , char * s , int radix , int max , bool upper) {
		_itoa(v , s , radix);
		if (!*s) strcpy(s , "0");
		if (max>0 && strlen(s)!=(unsigned)max) {
			int sz=strlen(s);
			if (sz>max) {
				for (int i=0;i<max;i++) {
					s[i]=s[sz-max+i];
				}
			} else {
				for (int i=max-1;i>=0;i--) {
					//      s[i]='E';
					if (i<max-sz) s[i]='0';
					else s[i]=s[i-max+sz];
				}
			}
			s[max]='\0';

		}
		if (radix>10 && upper) strupr(s);
		return s;
	}

/*	const char * inttoch(int v , int radix , int max , bool upper) {
		return inttoch(v , TLS().buff , radix , max , upper);
	}*/

	std::string inttostr(int v , int radix , int max , bool upper) {
		string s;
		inttoch(v, stringBuffer(s, 16) , radix,max,upper);
		stringRelease(s);
		return s;
	}

/*
	template <typename NUMBER, typename CHAR> NUMBER strToNumber(const CHAR * str , unsigned char base) {
		if (!str) return 0;
		bool sign=false; // ujemna
		// sprawdzamy znak ujemnoœci, który zawsze jest PRZED ew. definicj¹ bazy...
		if (*str == '-') {sign = true; str++;}
		// Sprawdzamy base..
		if (base == 0xFF) {
			if (str[0] == '#' || (str[0]=='0' && str[1]=='x')) {
				str+=(str[0] == '#')?1 : 2;
				base = 16;
			} else base = 10;
		}
		const char * last = str;
		// zliczamy od koñca, który najpierw trzeba znaleŸæ...
		while ((*last>='0' && *last<='9') || (*last>='a' && *last<='f') || (*last>='A' && *last<='F')) last++;
		if (last == str) return 0;
		last--; // Cofamy do pierwszego znaku...
		NUMBER l = 0; // liczba wyjsciowa
		NUMBER b = 1;  // base do potegi 0
		while (last >= str) { // wczytuje znaki od konca
			l += chval(*last)*b; // dodaje do l wartosc znaku * podstawa podniesiona do potegi
			b *= base; // "podnosi" base o potege
			last --;
		}
		return sign? -l : l;
	}
*/
	inline int checkIntChar(const char*& str, unsigned char& base) {
		int sign = 1;
		if (*str == '-') {sign = -1; str++;}
		// Sprawdzamy base..
		if (base == 0xFF) {
			if (str[0] == '#' || (str[0]=='0' && str[1]=='x')) {
				str += (str[0] == '#') ? 1 : 2;
				base = 16;
			} else base = 10;
		}
		return sign;
	}
	inline int checkIntChar(const wchar_t*& str, unsigned char& base) {
		int sign = 1;
		if (*str == L'-') {sign = -1; str++;}
		// Sprawdzamy base..
		if (base == 0xFF) {
			if (str[0] == L'#' || (str[0] == L'0' && str[1] == L'x')) {
				str += (str[0] == L'#') ? 1 : 2;
				base = 16;
			} else base = 10;
		}
		return sign;
	}

	int chtoint(const char * str , unsigned char base) {
		int sign = checkIntChar(str, base);
		return sign * strtoul(str, 0, base);
		//return strToNumber<unsigned int>(str, base);
	}
	__int64 chtoint64(const char * str , unsigned char base) {
		int sign = checkIntChar(str, base);
		//return strToNumber<unsigned __int64>(str, base);
		return sign * _strtoui64(str, 0, base);
	}
	int chtoint(const wchar_t * str , unsigned char base) {
		int sign = checkIntChar(str, base);
		return sign * wcstoul(str, 0, base);
		//return strToNumber<unsigned int>(str, base);
	}
	__int64 chtoint64(const wchar_t * str , unsigned char base) {
		int sign = checkIntChar(str, base);
		//return strToNumber<unsigned __int64>(str, base);
		return sign * _wcstoui64(str, 0, base);
	}


	char * str_tr(char * str , const char * chIn , const char * chOut) {
		if (!str || !chIn || !chOut || !*chIn || !*chOut || strlen(chIn)!=strlen(chOut)) return str;
		char * c = str;
		while (*c) {
			char * pos = strchr(chIn , *c);
			if (pos) {
				*c = chOut[pos - chIn];
			}
			c++;
		}
		return str;
	}


	#ifdef __BORLANDC__
	#define VSNPRINTF vsnprintf
	#else
	#define VSNPRINTF _vsnprintf
	#endif

	char * _vsaprintf(const char *format, va_list ap)
	{
			va_list temp_ap = ap;
			char *buf = NULL, *tmp;
			int size = 0, res;

			if ((size = VSNPRINTF(buf, 0, format, ap)) < 1) {
					size = 128;
					do {
							size *= 2;
							if ((tmp = (char*)realloc(buf, size))==0) {
									free(buf);
									return NULL;
							}
							buf = tmp;
							res = VSNPRINTF(buf, size, format, ap);
					} while (res == size - 1 || res==-1);
			} else {
					if ((buf = (char*)malloc(size + 1))==0)
							return NULL;
			}                                    

			ap = temp_ap;

			VSNPRINTF(buf, size + 1, format, ap);

			return buf;
	}

	char * _saprintf(const char *format, ...) {
		va_list ap;

		va_start(ap, format);
		char * ret = _vsaprintf(format, ap);
		va_end(ap);
			return ret;
	}




void split(const std::string & txt, const std::string & splitter, tStringVector & list, bool all) {
	std::string::size_type start = 0, end;
	list.clear();
	while (start < txt.length()) { // dopóki jest co kopiowaæ
		end = txt.find(splitter, start);
		if (all || start != end)
			list.push_back(txt.substr(start, (end == txt.npos? end : end - start)) );
		if (end == txt.npos)
			break;
		start = end + splitter.length();
	}
}
void splitCommand(const string & txt , char splitter ,  tStringVector & list) {
    if (txt.empty()) return;
    char end;
	size_t pos = 0;
	do {
	    // Ustalamy jakim znakiem ma sià zakonczyc parametr
	    if (splitter && txt[pos]=='"') {pos++; end = '"';}
        else end = splitter;
	    size_t fnd = txt.find(end , pos);
	    list.push_back(txt.substr(pos , (fnd != txt.npos)?fnd-pos:fnd));
	    pos = fnd;
	    if (pos != txt.npos) {
		    pos += (end=='"')?2:1;
            if (pos>=txt.size()) break;
	    } else break;
	} while (1);
}


	char * stripSlashes(char * str) {
		char * buff = str;
		int offset = 0;
		while (*buff) {
			if (*buff == '\\') {
				char ch = buff[1]; // jaki jest nastêpny?
				switch (ch) {
					case 'r': ch = '\r'; break;
					case 'n': ch = '\n'; break;
					case 't': ch = '\t'; break;
				}
				*(buff - offset) = ch;
				offset++;
                buff++;				
			} else {
				if (offset)
					*(buff - offset) = *buff;
			}
			buff++;
		}
		*(buff - offset) = *buff;
		return str;
	}


	std::string urlEncode(const std::string& str , char special , char * noChange) {
		string res;
		string::const_iterator str_it;
		res.reserve(str.size()+30);
		char buff [4];
		for (str_it=str.begin(); str_it != str.end(); str_it++) {
			if ((!noChange || !strchr(noChange , *str_it))&&(*str_it<'0'||*str_it>'9')&&(*str_it<'a'||*str_it>'z')&&(*str_it<'A'||*str_it>'Z')) {
				res.append(1 , special);
				itoa((unsigned char)*str_it , buff , 16);
				if (!buff[1]) {buff[2]=0;buff[1]=buff[0];buff[0]='0';}
				res.append(buff);
			} else {res.append(1, *str_it);}
		}
		return res;
	}

	std::string urlDecode(const std::string& in , char special) {
		std::string result = in;
		for (unsigned int i = 0; i<result.size(); i++) {
			if (result[i]==special) {
				result[i]=chtoint(result.substr(i+1,2).c_str() , 16);
				result.erase(i+1 , 2);
			}
		}
		return result;
	}

	std::string addSlashes(const std::string& str , char* escape, char escapeChar) {
		string res;
		string::const_iterator str_it;
		res.reserve(str.size()+30);
		for (str_it = str.begin(); str_it != str.end(); ++str_it) {
			if (strchr(escape, *str_it) != 0) {
				res.append(1 , escapeChar);
			}
			res.append(1, *str_it);
		}
		return res;
	}


	void * memrev(void * buff , int count) {
		char t;
		char * buf = (char*)buff;
		for (int i = 0 ; i < count / 2 ; i++) {
			t = buf[i];
			buf[i] = buf[count - i - 1];
			buf[count - i - 1] = t;
		}
		return buff;
	}

	std::string longToIp(long adr) {
		memrev(&adr , sizeof(adr));
		return stringf("%u.%u.%u.%u" , (adr&0xFF000000)>>24 , (adr&0xFF0000)>>16 , (adr&0xFF00)>>8 , adr&0xFF);
	}

	int ipToLong(const char * ip) {
		int a , b , c , d;
		sscanf(ip , "%u.%u.%u.%u" , &a , &b , &c , &d);
		return ((BYTE)d << 24) | ((BYTE)c<<16) | ((BYTE)b<<8) | (BYTE)a;
	}


// directories --------------------------------------------------

	int removeDirTree(const std::string& path) {
		if (path.empty()) return 0;
		WIN32_FIND_DATA fd;
		HANDLE hFile;
		BOOL found;
		found = ((hFile = FindFirstFile((path + "\\*.*").c_str(), &fd))!=INVALID_HANDLE_VALUE);
		//   int i = 0;
		int c = 1;
		while (found)
		{
			if (*fd.cFileName != '.') {
				std::string file = path + "\\";
				file += fd.cFileName;
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					c += removeDirTree(file);
				} else {
					unlink(file.c_str());
					c++;
				}
			}
			if (!FindNextFile(hFile , &fd)) break; 
		}
		FindClose(hFile);
		_rmdir(path.c_str());
		return c;
	}


	int createDirectories(const std::string& path) {
		if (path.empty()) return 0;	
		int c = 0;
		if (_access(path.c_str() , 0) != 0) { // nie ma katalogu...
			size_t slash = path.find_last_of('\\');
			if (slash != -1) {
				c = createDirectories(path.substr(0, slash));
			}
			if (_mkdir(path.c_str())) {
				return c + 1;
			}
		}
		return c;
	}

	bool fileExists(const char* file) {
		return _access(file, 0) == 0;
	}

	bool isDirectory(const char* path) {
		if (path==".") return true;
		DWORD attr = GetFileAttributes(path);
		if (attr == INVALID_FILE_ATTRIBUTES) 
			return false;
		else
			return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	std::string unifyPath(const std::string& path, bool slashSuffix, char delimiter) {
		if (path.empty()) return "";
		std::string res = path;

		for (std::string::iterator it = res.begin(); it != res.end(); ++it) {
			if (*it != delimiter && (*it == '/' || *it == '\\')) 
				*it = delimiter;
		}

		if (res[path.length() - 1] == delimiter) {
			if (slashSuffix == false) {
				res.erase(path.length() - 1);
			}
		} else {
			if (slashSuffix) {
				res += delimiter;
			}
		}
		return res;
		
	}


};