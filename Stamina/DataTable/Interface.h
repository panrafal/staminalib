#pragma once
/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: $
 */

namespace Stamina { namespace DT {

	class FileBase;

	class iInterface: public iSharedObject {

	public:

		STAMINA_OBJECT_CLASS(DT::iInterface, iSharedObject);

	public:

		virtual bool handleFailedLoad(FileBase* file, DTException* exception, int retry)=0;
		virtual bool handleFailedSave(FileBase* file, DTException* exception, int retry)=0;
		virtual bool handleFailedAppend(FileBase* file, DTException* exception, int retry)=0;

	};

	typedef SharedPtr<iInterface> oInterface;

} } 