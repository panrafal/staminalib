// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#ifdef _USE_PCH


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x600
#define _WIN32_WINDOWS 0x0490

#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <commctrl.h>
#include <stdstring.h>
#include <boost\signals.hpp>

#endif
