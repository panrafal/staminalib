/*

The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License from
/LICENSE.HTML in this package or at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is "Stamina.lib" library code, released Feb 1, 2006.

The Initial Developer of the Original Code is "STAMINA" - Rafa³ Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa³ Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id: $

*/

#pragma once

namespace Stamina { namespace DT {

	class FileBase;

	class iInterface: public iSharedObject {

	public:

		STAMINA_OBJECT_CLASS(DT::iInterface, iSharedObject);

		enum Result {
			retry = 1,
			fail = 2,
			quiet = 0x100,
			failQuiet = fail | quiet,
		};

	public:

		virtual Result handleFailedLoad(FileBase* file, DTException& exception, int retry)=0;
		virtual Result handleFailedSave(FileBase* file, DTException& exception, int retry)=0;
		virtual Result handleFailedAppend(FileBase* file, DTException& exception, int retry)=0;

		virtual void showFileMessage(FileBase* file, const StringRef& message, const StringRef& title, bool error)=0;
		virtual Result confirmFileError(FileBase* file, const StringRef& message, const StringRef& title, DTException& e)=0;

	};

	typedef SharedPtr<iInterface> oInterface;

} } 