/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once
#ifndef __STAMINA_CRITICAL_SECTION__
#define __STAMINA_CRITICAL_SECTION__

#include <windows.h>
#include "Lock.h"
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


	// =========================================================================

	// struktura do bezposredniej obslugi blokowania dostepu
	class CriticalSection_w32:public Lock {
	public:
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

	class CriticalSection_blank:public Lock {
	public:
		__inline void lock() {} 
		__inline void unlock() {} 
		int getLockCount() {return 0;}
		__inline bool canAccess() {return true;}
	};

	// struktura do bezposredniej obslugi blokowania dostepu
	class CriticalSection_:public Lock {
	public:
		CriticalSection_();
		~CriticalSection_();
		void lock(); // Zwraca liczbê wczeœniej za³o¿onych blokad
		void lock(DWORD timeout , char action=0); // Z TimeOutem
		void lock(DWORD timeout , char * where); // TA_EXCEPT z parametrem
		void unlock(); // Zwraca liczbê pozosta³ych blokad
		bool canAccess(); // Sprawdza, czy nie ma blokady
		void access(); // czeka tylko a¿ bêdzie móg³ wejœæ, bez blokowania
		int getLockCount() {return count;}
	protected:
		DWORD currentThread;    // ID aktualnie blokuj¹cego w¹tku
		unsigned short count; // iloœæ za³o¿onych bloków
		HANDLE event;

	};


	// Domyœlnie robi timeout...
	class CriticalSection_to: public CriticalSection_ {
	public:
		char * place;
		CriticalSection_to(char * place=""):place(place){}
		void lock() {return CriticalSection_::lock(5000 , place);}
	};


	// Z zabezpieczeniem na windowsowe komunikaty...
	typedef void (*tWMProcess)(void);
	class CriticalSection_WM: public CriticalSection_ {
	private:
	public:
		CriticalSection_WM(tWMProcess WMProcess = 0):_WMProcess(WMProcess) {}
		void lock(void);
	protected:
		tWMProcess _WMProcess;

	};

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