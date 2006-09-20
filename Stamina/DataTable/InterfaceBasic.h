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

#pragma once

#include "Interface.h"

namespace Stamina { namespace DT {

	class Interface_silent: public SharedObject<iInterface> {

	public:

		virtual Result handleFailedLoad(FileBase* file, DTException& exception, int retry) {
			return iInterface::fail;
		}
		virtual Result handleFailedSave(FileBase* file, DTException& exception, int retry) {
			return iInterface::fail;
		}
		virtual Result handleFailedAppend(FileBase* file, DTException& exception, int retry) {
			return iInterface::fail;
		}

		virtual void showFileMessage(FileBase* file, const StringRef& message, const StringRef& title, bool error) {
		}
		virtual Result confirmFileError(FileBase* file, const StringRef& message, const StringRef& title, DTException& e) {
			return iInterface::fail;
		}

	};

	class Interface_basic: public SharedObject<iInterface> {

	public:

		void showFileMessage(FileBase* file, const StringRef& _message, const StringRef& _title, bool error) {
			String msg;
			msg = error ? L"Wyst¹pi³ b³¹d w pliku " : L"Wyst¹pi³ problem z plikiem ";
			msg += "\"" + file->getFilename() + "\"\r\n\r\n";
			msg += _message; 

			String title = _title;
			title += " (" + getFileName(file->getFilename()) + ")";

			MessageBoxW(0, msg.w_str(), title.w_str(), MB_OK | (error ? MB_ICONERROR : MB_ICONWARNING));
		}

		Result showConfirmFileError(const StringRef& message, const StringRef& title) {
			int r = MessageBoxW(0, message.w_str(), title.w_str(), MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON2);
			if (r == IDIGNORE) {
				return iInterface::fail;
			} else if (r == IDABORT) {
				abort();
			}
			return iInterface::retry;
		}


		Result confirmFileError(FileBase* file, const StringRef& message, const StringRef& _title, DTException& e) {
			String msg;
			msg = L"Wyst¹pi³ b³¹d w pliku ";
			msg += "\"" + file->getFilename() + "\"\r\n\r\n";
			msg += message; 
			msg += L"\r\n\r\nMo¿esz ponowiæ próbê, zignorowaæ dane zawarte w pliku, lub wyjœæ z programu.";

			String title = _title;
			title += " (" + getFileName(file->getFilename()) + ")";

			Result res = showConfirmFileError(msg, title);

			if (res == iInterface::fail) {
				return showConfirmFileError("Uwaga! Zignorowanie b³êdu mo¿e doprowadziæ do utraty danych! Jesteœ pewien?", title);
			}
			return res;
		}

		Result handleMessageErrors(FileBase* file, DTException& e, const StringRef& title) {
			String msg;
			bool confirm = true;
			switch (e.errorCode) {
				case DT::errBadFormat: msg = L"Z³y format pliku"; break;
				case DT::errBadParameter: msg = L"Z³y parametr"; break;
				case DT::errBadType: msg = L"Z³y typ"; break;
				case DT::errNotAuthenticated: msg = L"Z³e has³o!"; break;
				case DT::errBadVersion: msg = L"Nieobs³ugiwana wersja pliku - "; msg += file->getVersion().getString(); break;
				//case DT::errFileNotFound: msg = L"Plik nie zosta³ znaleziony"; break;
				case DT::errWriteError: msg = L"B³¹d zapisu"; break;
				default:
					if (e.errorCode & errFileError) {
						msg = L"B³¹d systemu plików - "; msg += inttostr(e.errorCode & ~DT::errFileError);			
					}

			}
			if (msg.empty() == false) {
				if (confirm) {
					return this->confirmFileError(file, msg, title, e);
				} else {
					this->showFileMessage(file, msg, title, true);
				}
			}
			return iInterface::fail;
		}

		virtual Result handleFailedLoad(FileBase* file, DTException& e, int retry) {
			if (e.errorCode == DT::errNotAuthenticated) {
				Result res = this->handleNotAuthenticated(file, e, retry);
				if (res != iInterface::fail) {
					return res;
				}
			} else if (file->getClass() >= FileBin::staticClassInfo()) {
				FileBin* fb = file->castObject<FileBin>();
				if ((e.errorCode == DT::errBadFormat || e.errorCode & DT::errFileError) && fb->makeBackups) {
					return this->handleRestoreBackup(fb, e, retry);
				}
			}
			return handleMessageErrors(file, e, "B³¹d podczas wczytywania pliku");
		}

		virtual Result handleFailedSave(FileBase* file, DTException& e, int retry) {
			if (e.errorCode == DT::errNotAuthenticated) {
				Result res = this->handleNotAuthenticated(file, e, retry);
				if (res != iInterface::fail) {
					return res;
				}
			} else {
			}
			return handleMessageErrors(file, e, "B³¹d podczas zapisywania pliku");
		}

		virtual Result handleFailedAppend(FileBase* file, DTException& e, int retry) {
			if (e.errorCode == DT::errNotAuthenticated) {
				Result res = this->handleNotAuthenticated(file, e, retry);
				if (res != iInterface::fail) {
					return res;
				}
			} else if (file->getClass() >= FileBin::staticClassInfo()) {
				FileBin* fb = file->castObject<FileBin>();
				if (e.errorCode == DT::errBadFormat && fb->makeBackups) {
					Result res = this->handleRestoreBackup(fb, e, retry);
					if (res == iInterface::failQuiet || res == iInterface::retry) {
						return res;
					}
				}
			}
			return handleMessageErrors(file, e, "B³¹d podczas dopisywania do pliku");
		}


		virtual Result handleNotAuthenticated(FileBase* file, DTException& e, int retry) {
			return iInterface::fail;
		}

		virtual Result handleRestoreBackup(FileBin* file, DTException& e, int retry) {
			return file->restoreLastBackup() ? iInterface::retry : iInterface::fail;
		}


	};


	class Interface_passList: public Interface_basic {
	public:

		typedef std::list<MD5Digest> tDigests;

		Interface_passList() {
			this->addPassword("");
		}

	public:

		virtual Result handleNotAuthenticated(FileBase* file, DTException& e, int retry) {
			// próbujemy wszystkie z listy...
			MD5Digest oldDigest = file->getDT()->getPasswordDigest();
			for (tDigests::iterator it = _digests.begin(); it != _digests.end(); ++it) {
				file->getDT()->setPasswordDigest(*it);
				if (file->isAuthenticated()) {
					return iInterface::retry;
				}
			}
			// trzeba zapytaæ usera o has³o...
			MD5Digest newDigest = this->askForPassword(file, retry);
			if (newDigest.empty() == false) {
				file->getDT()->setPasswordDigest(newDigest);
				if (file->isAuthenticated()) {
					// dzia³a! dopisujemy do listy... na pewno go na niej nie ma...
					_digests.push_back(newDigest);
					return iInterface::retry;
				}
			}
			// nic siê nie uda³o...
			file->getDT()->setPasswordDigest(oldDigest);
			return iInterface::fail;
		}

		virtual MD5Digest askForPassword(FileBase* file, int retry) {
			return MD5Digest();
		}

		void addPassword(const StringRef& pass) {
			this->addDigest(DataTable::createPasswordDigest(pass));
		}

		void addDigest(const MD5Digest& digest) {
			if (std::find(_digests.begin(), _digests.end(), digest) == _digests.end()) {
				_digests.push_back(digest);
			}
		}

		tDigests& getDigests() {
			return _digests;
		}

	private:

		tDigests _digests;

	};


} } 