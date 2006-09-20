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

$Id$

*/

#pragma once

/**@file TLS - Thread Local Storage
*/

namespace Stamina {

	/** Class for easy storage/retrieval of local thread variables

	Usage:
	- Build a class that contains all variables you want to have stored separately for every thread.
	- Create one object for all threads of type TLS<yourClass>
	- Use operator () to retrieve (with automatic creation) an object for current thread

	@codebegin
	class example {
		public: int a;
	};
	TLS<a> tls;
	tls().a = 0;
	@codeend

	You should call TLS::detach before thread is destroyed! Otherwise there will be an unused object hanging in memory!
	*/

	template <class T> class ThreadLocalStorage {
	public:

		/**Returns a reference to the current thread's object.
		If there is no object already, it's automatically created with default constructor.
		*/
		T & operator ()(void) {
			T * t = reinterpret_cast<T*>( TlsGetValue( tls_index ) );
			if (!t) {
				t = new T;
				TlsSetValue(tls_index, t);
			}
			return *t;
		}

		/**Destroys the current thread's object.
		@warning After calling detach, there should be no calls to TLS::() !
		*/
		void detach(void) {
			T * t = reinterpret_cast<T*>( TlsGetValue(tls_index) );
			if (t) {
				delete t; 
				TlsSetValue(tls_index, 0);
			}
		}

		ThreadLocalStorage() {
	        tls_index = TlsAlloc();
		}

		~ThreadLocalStorage() {
			detach();
			TlsFree(tls_index);
		}
	private:
		unsigned long tls_index;
	};

};