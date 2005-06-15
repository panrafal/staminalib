/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#include "stdafx.h"
#include "DataTable.h"

namespace Stamina { namespace DT {

	FileBase::FileBase() {
		fcols.loader = true; 
		opened=0;
		write_failed = false;
	}
	FileBase::FileBase(DataTable * t) {fcols.loader = true; opened=0;assign(t);}
	void FileBase::assign(DataTable * t) {table=t;fcols.table=t;}
	FileBase::~FileBase() {}


    int FileBase::load (const char * fn)
    {
      if (!fn) fn=fileName.c_str();
       else fileName = fn;
      table->clearrows();
      fcols.clear();
//      open(fn , DT_READ);
      int r;
      r=open(fn , DT_READ);
      if (r) return r;
      r=freaddesc(); if(r) return r;
      r=readrows(); if(r) return r;
      close();
      return 0;
    }

    int FileBase::loadAll (const char * fn)
    {
      if (!fn) fn=fileName.c_str();
       else fileName = fn;
      table->clearrows();
      fcols.clear();
//      open(fn , DT_READ);
      int r;
      r=open(fn , DT_READ);
      if (r) return r;
      r=freaddesc(); if(r) return r;
	  table->cols.join(&fcols , false);
      r=readrows(); if(r) return r;
      close();
      return 0;
    }


    int FileBase::save (const char * fn)
    {
      if (!fn) fn=fileName.c_str();
       else fileName = fn;
      fcols = table->cols;
      int r;
      r=open(fn , DT_WRITE);
      if (r) return r;
      fwritedesc();
      for (unsigned int i=0; i < table->getrowcount() ; i ++)
        {if (!(table->rows[i]->flag & DT_RF_DONTSAVE)) fwriterow(i);}
      close();
      return 0;
    }

    int FileBase::append (const char * fn) {
      if (!fn) fn=fileName.c_str();
       else fileName = fn;
      fcols.clear();
      int r;
      bool first = _access(fn , 0)!=0;
      r=open(fn , DT_APPEND);
      if (r) return r;
      fset(0 , SEEK_SET);
      if (first) {
        fcols = table->cols;
        table->lastid = DT_ROWID_MIN;
        fwritedesc();
      } else {
        r=freaddesc(); if(r) return r;
      }
/*      close();
      r=open(fn , DT_APPEND);*/
      if (r) return r;
      fset(0 , SEEK_END);
      for (unsigned int i=0; i < table->getrowcount() ; i ++)
        {table->lastid++;
         if (table->lastid > DT_ROWID_MAX) table->lastid = DT_ROWID_MIN;
         table->rows[i]->id=table->lastid;
         fwriterow(i);}

      close();
      return 0;
    }


} }