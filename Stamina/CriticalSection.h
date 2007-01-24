/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once
#ifndef __STAMINA_CRITICAL_SECTION__
#define __STAMINA_CRITICAL_SECTION__

#include "Stamina.h"
#include <windows.h>
#include "Lock.h"
#include "Assert.h"
/*
cCriticalSection to bardzo prosta, obiektowa, niekoniecznie szybsza
wersja obslugi sekcji krytycznych. Dodatkowa opcja, jest mozliwosc
timeout'owania oczekiwania sekcji wyjatkiem strukturalnym.
*/

#define CRITICAL_SECTION_TIMEOUT 5000
#define CRITICAL_SECTION_TIMEOUT_EXCEPTION STATUS_TIMEOUT | 0x1000


#define CRITICAL_SECTION_TA_BREAK (char)0
#define CRITICAL_SECTION_TA_EXCEPT (char)1
#define CRITICAL_SECTION_TA_EXCEPT2 (char)2 // STATUS_TIMEOUT
#define CRITICAL_SECTION_TA_THROW (char)3

namespace Stamina {


	/** Empty Critical section class.
	*/
	class iCriticalSection:public Lock {
	public:

		STAMINA_OBJECT_CLASS(iCriticalSection, Lock);

	private:

		virtual void zzPlaceHolder_iCriticalSection1() {}
		virtual void zzPlaceHolder_iCriticalSection2() {}
		virtual void zzPlaceHolder_iCriticalSection3() {}
		virtual void zzPlaceHolder_iCriticalSection4() {}

	};


	// =========================================================================

	// struktura do bezposredniej obslugi blokowania dostepu
	/** Simple class to direct use of blocking access.
	*/
	class CriticalSection_w32:public iCriticalSection {
	public:

		STAMINA_OBJECT_CLASS(CriticalSection_w32, iCriticalSection);

		CRITICAL_SECTION cs;

		CriticalSection_w32();
		~CriticalSection_w32();
		__inline void lock() {	EnterCriticalSection(&cs);} 
		//__inline bool tryLock() {return TryEnterCriticalSection(&cs);} 
		__inline void unlock() {LeaveCriticalSection(&cs);} 
		__inline bool canAccess() {
			return cs.LockCount<=0 || cs.OwningThread == GetCurrentThread();
		}
		int getLockCount() {return cs.LockCount;}

	};

	class CriticalSection_simple: public iCriticalSection {
	public:

		STAMINA_OBJECT_CLASS(CriticalSection_simple, iCriticalSection);

		__inline CriticalSection_simple() {
			_occupied = 0;
			_thread = 0;
		}
		void lock() {
			LONG current = GetCurrentThreadId();
			LONG result;
			while ((result = InterlockedCompareExchange(&_thread, current, 0)) != current && result != 0){
				Sleep(1);
			}
			InterlockedIncrement(&_occupied);
		}
		void unlock() {
			if (InterlockedDecrement(&_occupied) == 0) {
				LONG current = GetCurrentThreadId();
				S_ASSERT_RUN( InterlockedCompareExchange(&_thread, 0, current) == current );
			}
		}
		bool canAccess() {
			return _occupied != 1;
		}
		int getLockCount() {
			return _occupied;
		}
	private:
        volatile LONG _occupied;
		volatile LONG _thread;
	};


	/** Empty Critical section class.
	*/
	class CriticalSection_blank:public iCriticalSection {
	public:

		STAMINA_OBJECT_CLASS(CriticalSection_blank, iCriticalSection);

		__inline void lock() {} 
		__inline void unlock() {} 
		int getLockCount() {return 0;}
		__inline bool canAccess() {return true;}
	};

	// struktura do bezposredniej obslugi blokowania dostepu
	/** Class to direct use of blocking access.
	*/
	class CriticalSection_:public iCriticalSection {
	public:

		STAMINA_OBJECT_CLASS(CriticalSection_, iCriticalSection);

		CriticalSection_();
		~CriticalSection_();
		void lock(); // Zwraca liczbê wczeœniej za³o¿onych blokad
		void lock(DWORD timeout , char action=0); // Z TimeOutem
		void lock(DWORD timeout , char * where); // TA_EXCEPT z parametrem
		void unlock();
		bool canAccess(); // Sprawdza, czy nie ma blokady
		void access(); // czeka tylko a¿ bêdzie móg³ wejœæ, bez blokowania
		int getLockCount() {return count;}
	protected:
		DWORD currentThread;    // ID aktualnie blokuj¹cego w¹tku
		unsigned short count; // iloœæ za³o¿onych bloków
		HANDLE event;

	};


	// Domyœlnie robi timeout...

	/** Critical section with time-outs.
	*/
	class CriticalSection_to: public CriticalSection_ {
	public:
		char * place;
		CriticalSection_to(char * place=""):place(place){}
		void lock() {return CriticalSection_::lock(5000 , place);}
	};


	// Z zabezpieczeniem na windowsowe komunikaty...
	/** Critical Section whith protection from windows messages.
	*/
	typedef void (*tWMProcess)(void);
	class CriticalSection_WM: public CriticalSection_ {
	private:
	public:
		CriticalSection_WM(tWMProcess WMProcess = 0):_WMProcess(WMProcess) {}
		void lock(void);
	protected:
		tWMProcess _WMProcess;

	};

	/** @todo what is CriticalSection_WM_cond ?? */
	class CriticalSection_WM_cond: public CriticalSection_WM {
	private:
	public:
		CriticalSection_WM_cond(int forThread, tWMProcess WMProcess = 0):CriticalSection_WM(WMProcess), forThread(forThread) {}
		void lock(void);
	protected:
		int forThread;
	};


	typedef CriticalSection_w32 CriticalSection;
	//typedef CriticalSection CriticalSection;




};
#endif