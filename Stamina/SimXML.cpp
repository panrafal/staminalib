/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa≥ Lindemann, Stamina
 */

#include "stdafx.h"
#include <windows.h>
#include "SimXML.h"
#include <stdstring.h>
#include <stdlib.h>
#include <stack>


using namespace std;

namespace Stamina {

	SXML::SXML() {
//		pregPath = false;
	}

	SXML::Position::Position() {
		end = end_end = start = start_end = -1;
	}
	SXML::~SXML() {
	}

	bool SXML::loadFile(const char *  fileName) {
		string _src="";
		FILE * f = fopen (fileName , "rt");
		if (!f) return false;
		char temp [251];
		size_t r;
		while ((r = fread(temp , 1 , 250 , f))) {
			temp[r]=0;
			_src+=temp;
		}
		fclose(f);
		return loadSource(_src.c_str());
	}

	bool SXML::loadSource(const char *  xml) {
		pos = Position();
		preg.setSubject(xml);
		return *xml != 0;
	}

	string SXML::getSource() {
		return preg.getSubject();
	}

	string SXML::getContent(const char * path){
		if (path) prepareNode(path , true);
		if (pos.start_end == -1 || pos.end==-1) return "";
		return preg.getSubject(pos.start_end , pos.end);
	}
	string SXML::getNode(const char * path){
		if (path) prepareNode(path , true);
		if (pos.start == -1 || pos.end_end==-1) return "";
		return preg.getSubject(pos.start , pos.end_end);
	}

	string __stdcall encodeCallback(class RegEx * p , void * param) {
		char v [7] = "&#\0\0\0\0";
		itoa(*p->getByVector(0) , v+2 , 10);
		v[strlen(v)]=';';
		return v;
	}
	string __stdcall decodeCallback(class RegEx * p , void * param) {
		string fnd = (*p)[1];
		if (fnd=="amp") return "&";
		else if (fnd=="lt") return "<";
		else if (fnd=="gt") return ">";
		else if (fnd=="quot") return "\"";
		else if (fnd=="apos") return "'";
		char v [2] = " ";
		v[0] = char(strtol(fnd.c_str() , 0 , *(p->getByVector(0)+2) == 'x'?16:10));
		return v;
	}

	string encodeEntities(string txt) {
		RegEx pr;
		return pr.replace("/[^ \\t\\r\\na-z0-9\\!\\@\\#\\$\\%\\*\\(\\)\\-_=+\\.\\,\\;':\\\\[\\]\\{\\}πÊÍ≥ÒÛúüø•∆ £—”åèØ]/i" , encodeCallback , txt.c_str());
	}
	string decodeEntities(string txt) {
		RegEx pr;
		return pr.replace("/&#?x?([0-9a-z]+);/i" , decodeCallback , txt.c_str());
	}

	string SXML::getText(const char * path){
		if (path) prepareNode(path , true);
		if (pos.start_end == -1 || pos.end==-1) return "";
		string node = preg.getSubject(pos.start_end , pos.end);
		RegEx pr;
		pr.match("/[ \n]*<![CDATA[(.*?)]]>/m",node.c_str());
		CStdString str;
		if (pr.isMatched()) {str = preg[1];}
		else str = pr.replace("/<.+>/m","",node.c_str());
		//    str = pr.replace("");
		str = decodeEntities(str);
		return str;
	}

	bool SXML::nodeWalk(SXML::NodeWalkInfo & nfo) {
		if (nfo.state == nfo.closed || nfo.state == nfo.closing) {
			// Obniøamy "element"
			size_t top = nfo.path.find_last_of("/");
			if (top == -1) nfo.path = "";
			else nfo.path.erase(top);
		}
		// 1 - <(/)...   2 - (nazwa)  3 - ...(/)>
		preg.setPattern("#<(?!\\?|\\!)[ \\t]*(/?)([^>/ \\t\\n\\r]+)(?=\\t| |\\n|>|\\r|/).*?(/?)>#is");
		preg.match();
		if (!preg.isMatched()) {
			nfo.state = nfo.undefined;
			return false;
		}
		nfo.start = preg.getVector(0);
		nfo.end = preg.getVector(1);
		preg.setStart(nfo.end); // Przesuwamy siÍ...
		if (preg[1]=="/" && nfo.path.substr(nfo.path.find_last_of("/")+1) == preg[2]) {//Zamykamy wczytany wczeúniej element
			nfo.state = nfo.closing;
		} else {
			if (!nfo.path.empty()) nfo.path+='/';
			nfo.path+=preg[2];
			if (preg[3]=="/") nfo.state = nfo.closed;
			else nfo.state = nfo.opened;
		}
		pos.end = preg.getVector(0);
		pos.end_end = preg.getVector(1);
		return true;
	}

	bool SXML::prepareNode(const string& path , bool full){
		if (path.empty()) return false;
		size_t lastStart = preg.getStart();
		NodeWalkInfo nfo;
		pos = Position();
		while (nodeWalk(nfo)) { // dopÛki wczytujemy kolejne elementy
			bool ok;
			if (path[0]=='*')
				ok = (nfo.path.size() >= path.size()-1) 
				&& (nfo.path.substr(nfo.path.size() - path.size() + 1) == path.c_str()+1);
			else ok = (nfo.path == path);
			if (ok && nfo.state != nfo.closing) {
				pos.start = nfo.start;
				pos.start_end = nfo.end;
				if (nfo.state == nfo.closed) {
					pos.end = pos.end_end = nfo.end;
				}
				break;
			}
		}
		if (full && nfo.state != nfo.closed && nfo.state != nfo.undefined) {
			// Szukamy do koÒca
			string toClose = nfo.path;
			while (nodeWalk(nfo)) {
				if (nfo.path == toClose && nfo.state == nfo.closing) {
					pos.end = nfo.start;
					pos.end_end = nfo.end;
					break;
				}
			}
		}
		if (nfo.state == nfo.undefined) {
			pos = Position();
		}
		preg.setStart(lastStart);
		return nfo.state != nfo.undefined;

		/*	
		string start("") , end("");
		size_t fpos = 0 , bpos = 0;
		do {
		fpos = path.find('/' , bpos);   // Wyluskujemy kolejne podkatalogi
		string found = path.substr(bpos , fpos==path.npos?path.npos : fpos - bpos);
		if (found == "") continue;
		if (start != "") start+="[^>]*>.*";  // Jeøeli juø coú znaleüliúmy, rozgraniczamy pola poprzez .*
		if (fpos == -1) // ostatni element
		start += "(<) *("+found+")";
		else
		start += "< *"+found;
		bpos = fpos + 1;
		} while (fpos != path.npos);
		// Znajdujemy pierwszy element...
		preg.setPattern("/" + start + "[^>]*?(\\/?)>/isU");
		preg.match();
		if (preg.matched) {
		pos.start = preg.getVector(2); // poczπtek pierwszego "(<)"
		pos.start_end = preg.getVector(1); // koniec ca≥oúci
		}
		else {
		pos = sPos(); // resetujemy pozycje...
		return false;
		}
		if (preg[3]=="/") { // Element zamkniÍty przez />
		pos.end = pos.start_end;
		pos.end_end = pos.start_end;
		return true;
		}
		// parsujemy aø znajdziemy koniec
		if (full) {
		} else {
		pos.end = pos.end_end = -1;
		}
		*/
	}

	string SXML::getAttrib(const char * attrib , const char * attribs){
		if (!attribs && pos.start==-1) return "";
		RegEx pregA;
		pregA.setPattern("/[ \\t]"+string(attrib)+"[ \\t]*=[ \\t]*([\"'])(.*?)\\1/is");
		pregA.setSubject(attribs ? attribs : preg.getSubject(pos.start , pos.start_end));
		pregA.match();
		return decodeEntities(pregA[2]);
	}
	string SXML::getAttribs(const char * path){
		if (path) prepareNode(path , true);
		if (pos.start == -1 || pos.start_end==-1) return "";
		if (pos.start==-1) return "";
		return preg.getSubject(pos.start , pos.start_end);
	}


	void SXML::next() {
		if (pos.start==-1) return;
		preg.setStart(pos.end_end!=-1 ? pos.end_end  :  pos.start_end); 
	}
	void SXML::restart() {preg.reset();} 


};



