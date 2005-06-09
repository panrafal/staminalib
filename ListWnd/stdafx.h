// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _USE_PCH

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _WIN32_WINNT 0x501
#include <windows.h>
#include <algorithm>
#include <boost\function.hpp>
#include <boost\bind.hpp>
#include <boost\signals.hpp>

#endif
