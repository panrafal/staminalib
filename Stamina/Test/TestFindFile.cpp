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

#include <stdafx.h>
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina/VersionControl.h>
#include "..\FindFileFiltered.h"
#include <Stamina\Helpers.h>
#include <Stamina\Time64.h>

using namespace Stamina;
using FindFile::Found;

using std::string;
using namespace std;

#include <Stamina\Console.h>

class TestFindFile : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestFindFile );
  
	CPPUNIT_TEST( testDirOnly );
	CPPUNIT_TEST( testFileOnly );
	CPPUNIT_TEST( testParentPointers );
	CPPUNIT_TEST( testMask );
	CPPUNIT_TEST( testRegEx );
	CPPUNIT_TEST( testNegRegEx );

    CPPUNIT_TEST_SUITE_END();

protected:
  

public:
	void setUp() {
	}
	void tearDown() {
	}

protected:

	void printFoundInfo(const Found& found) {
		cout << endl;
		cout << "Found: " << found.getFileName() << (found.isDirectory() ? "\\ " : " ");
		cout << found.getFileSize() << "B ";
		cout << Time64(found.getCreationTime()).strftime(" C:%d-%m-%Y %H:%M:%S");
		cout << Time64(found.getLastAccessTime()).strftime(" A:%d-%m-%Y %H:%M:%S");
		cout << Time64(found.getLastWriteTime()).strftime(" M:%d-%m-%Y %H:%M:%S");
		cout << endl;

	}

	void testDirOnly() {
		FindFile ff("c:\\*");
		ff.setDirOnly();
		ff.excludeAttribute(FindFile::attHidden);
		int count = 0;
		while (ff.find()) {
			//printFoundInfo(ff.found());
			CPPUNIT_ASSERT(ff.found().getFileName() != ".");
			CPPUNIT_ASSERT(ff.found().getFileName() != "..");
			CPPUNIT_ASSERT(ff->hasAttribute(FindFile::attHidden) == false);
			CPPUNIT_ASSERT(ff.found().isParentPointer() == false);
			CPPUNIT_ASSERT(ff.found().isDirectory() == true);
			CPPUNIT_ASSERT(Time64(ff.found().getCreationTime()).empty() == false);
			count++;
		}
		CPPUNIT_ASSERT( count > 1 );
		CPPUNIT_ASSERT(ff.nothingFound() == false);
	}

	void testFileOnly() {
		FindFile ff("c:\\*");
		ff.setFileOnly();
		ff.requireAttribute(FindFile::attHidden);
		int count = 0;
		while (ff.find()) {
			//printFoundInfo(ff.found());
			CPPUNIT_ASSERT(ff.found().getFileName() != ".");
			CPPUNIT_ASSERT(ff.found().getFileName() != "..");
			CPPUNIT_ASSERT(ff.found().isParentPointer() == false);
			CPPUNIT_ASSERT(ff.found().isDirectory() == false);
			CPPUNIT_ASSERT(ff->hasAttribute(FindFile::attHidden) == true);
			count++;
		}
		CPPUNIT_ASSERT( count > 1 );
		CPPUNIT_ASSERT(ff.nothingFound() == false);
	}

	void testParentPointers() {
		FindFile ff("c:\\windows\\*");
		ff.includeParent();
		CPPUNIT_ASSERT(ff.find());
		CPPUNIT_ASSERT(ff.found().isParentPointer() == true);
		CPPUNIT_ASSERT(ff.found().getFileName() == ".");
		CPPUNIT_ASSERT(ff.find());
		CPPUNIT_ASSERT(ff.found().isParentPointer() == true);
		CPPUNIT_ASSERT(ff.found().getFileName() == "..");
	}

	void testMask() {
		FindFileFiltered ff("c:\\Windows\\System32\\t*.exe");
		ff.setFileOnly();
		int count = 0;
		while (ff.find()) {
			//printFoundInfo(ff.found());
			CPPUNIT_ASSERT(ff.found().isParentPointer() == false);
			CPPUNIT_ASSERT(ff.found().isDirectory() == false);
			CPPUNIT_ASSERT(RegEx::doMatch("/^t.+\\.exe$/i", ff->getFileName().c_str()));
			count++;
		}
		CPPUNIT_ASSERT(ff.nothingFound() == false);
		CPPUNIT_ASSERT( count > 1 );
	}

	void testRegEx() {
		FindFileFiltered ff("c:\\Windows\\System32\\*.exe");
		ff.addFilter(new FileFilter_RegEx("/\\.exe$/i"));
		ff.addFilter(new FileFilter_RegEx("/^[^a]+$/i"));
		ff.setFileOnly();
		int count = 0;
		while (ff.find()) {
			//printFoundInfo(ff.found());
			CPPUNIT_ASSERT(ff.found().isParentPointer() == false);
			CPPUNIT_ASSERT(ff.found().isDirectory() == false);
			CPPUNIT_ASSERT(RegEx::doMatch("/^[^a]+\\.exe$/i", ff->getFileName().c_str()));
			count++;
		}
		CPPUNIT_ASSERT(ff.nothingFound() == false);
		CPPUNIT_ASSERT( count > 1 );
	}

	void testNegRegEx() {
		FindFileFiltered ff("c:\\Windows\\System32\\*.exe");
		ff.addFilter(new FileFilter_RegEx("/\\.exe$/i"));
		ff.addFilter(new FileFilter_RegEx("/^[^a]+$/i", false));
		ff.setFileOnly();
		int count = 0;
		while (ff.find()) {
			//printFoundInfo(ff.found());
			CPPUNIT_ASSERT(ff.found().isParentPointer() == false);
			CPPUNIT_ASSERT(ff.found().isDirectory() == false);
			CPPUNIT_ASSERT(RegEx::doMatch("/.*a.*\\.exe$/i", ff->getFileName().c_str()));
			count++;
		}
		CPPUNIT_ASSERT(ff.nothingFound() == false);
		CPPUNIT_ASSERT( count > 1 );
	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestFindFile );


