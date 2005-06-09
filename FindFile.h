/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once

namespace Stamina {

	
	class FindFile {
	public:

		FindFile(const std::string& mask);

		void includeAttribute(int attr);
		void excludeAttribute(int attr);

		void setDirOnly();
		void setFileOnly();

		~FindFile();

		/** Looks for first/next file. Returns false if nothing found */
		bool find();

	private:


	};


}