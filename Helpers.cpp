/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa� Lindemann, Stamina
 */

#include "stdafx.h"
#include <deque>

#include <stdstring.h>
#include "Helpers.h"


using namespace std;

namespace Stamina {

	struct __initializer {
		__initializer() {
			srand(time(0));
		}
	} _initializer;


	int random(int min, int max) {
		static bool seeded = false;
		if (!seeded) {
			randomSeed();
			seeded = true;
		}
		int r = rand();
		int width = RAND_MAX;
		if (max - min > width) {
			width = 0x3FFFFFFF;
			r = r | (rand() << 15);
			if (max - min > width) {
				width = 0xFFFFFFFF;
				r = r | (rand() << 30);
			}
		}
		r = round(r * ((double)(max - min) / width));
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

	int chtoint(const char * str , unsigned char base) {
		if (!str) return 0;
		bool sign=false; // ujemna
		// sprawdzamy znak ujemno�ci, kt�ry zawsze jest PRZED ew. definicj� bazy...
		if (*str == '-') {sign = true; str++;}
		// Sprawdzamy base..
		if (base == 0xFF) {
			if (str[0] == '#' || (str[0]=='0' && str[1]=='x')) {
				str+=(str[0] == '#')?1 : 2;
				base = 16;
			} else base = 10;
		}
		const char * last = str;
		// zliczamy od ko�ca, kt�ry najpierw trzeba znale��...
		while ((*last>='0' && *last<='9') || (*last>='a' && *last<='f') || (*last>='A' && *last<='F')) last++;
		if (last == str) return 0;
		last--; // Cofamy do pierwszego znaku...
		unsigned int l=0; // liczba wyjsciowa
		int b=1;  // base do potegi 0
		while (last >= str) { // wczytuje znaki od konca
			l+=chval(*last)*b; // dodaje do l wartosc znaku * podstawa podniesiona do potegi
			b*=base; // "podnosi" base o potege
			last --;
		}
		return sign? -(signed)l : (signed)l;
	}


void split(const std::string & txt, const std::string & splitter, tStringVector & list, bool all) {
	std::string::size_type start = 0, end;
	list.clear();
	while (start < txt.length()) { // dop�ki jest co kopiowa�
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
	    // Ustalamy jakim znakiem ma si� zakonczyc parametr
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
				char ch = buff[1]; // jaki jest nast�pny?
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


};