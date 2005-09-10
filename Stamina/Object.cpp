/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#include <boost\crc.hpp>

#include "Helpers.h"

#include "ObjectImpl.h"



namespace Stamina {


	//const unsigned int libInstance = random(0, 0xFFFFFFF);

	unsigned int ObjectClassInfo::getUID() {
		if (_uid) return _uid;
		else {
			boost::crc_32_type crc;
			for (const char * name = _name; *name; ++name) {
				crc.process_byte(*name);
			}
			return _uid = crc.checksum();
		}
	}


#ifdef STAMINA_DEBUG
	std::list<class iObject*>* debugObjects = new std::list<class iObject*>;
	CriticalSection* debugObjectsCS = new CriticalSection;

	class __initializer {
	public:
		__initializer() {
		}
		~__initializer() {
			if (debugObjects) {
				Locker locker (debugObjectsCS);
				// nie kasujemy debugObjectsCS, ¿eby nie rozwaliæ ew. czekaj¹cych destruktorów
				debugObjectsCS = 0;
				delete debugObjects;
				debugObjects = 0;
				
			}
		}
	} _initializer;

#endif


};