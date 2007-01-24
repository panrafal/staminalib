// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#include <iostream>
#include <tchar.h>
#include <conio.h>

#include <string>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h> 

#include <boost/function.hpp>
#include <boost/signal.hpp>
#include <boost\smart_ptr.hpp>


//#include <Stamina/VersionControl.h>

// TODO: reference additional headers your program requires here
