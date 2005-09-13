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

		enum Result {
			retry,
			fail,
			quiet = 0x100,
			failQuiet = fail | quiet,
		};

	public:

		virtual Result handleFailedLoad(FileBase* file, DTException* exception, int retry)=0;
		virtual Result handleFailedSave(FileBase* file, DTException* exception, int retry)=0;
		virtual Result handleFailedAppend(FileBase* file, DTException* exception, int retry)=0;

		virtual void showFileMessage(FileBase* file, const StringRef& message, const StringRef& title, bool error)=0;

	};

	typedef SharedPtr<iInterface> oInterface;

} } 