#pragma once
/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

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

	template <class T> class TLS {
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

		TLS() {
	        tls_index = TlsAlloc();
		}

		~TLS() {
			detach();
			TlsFree(tls_index);
		}
	private:
		unsigned long tls_index;
	};

};