#pragma once
/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: $
 */

#include "Interface.h"

namespace Stamina { namespace DT {

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

		Result confirmFileError(FileBase* file, const StringRef& message, const StringRef& _title, DTException* e) {
			String msg;
			msg = L"Wyst¹pi³ b³¹d w pliku ";
			msg += "\"" + file->getFilename() + "\"\r\n\r\n";
			msg += message; 
			msg += L"\r\n\r\nMo¿esz ponowiæ próbê, zignorowaæ dane zawarte w pliku, lub wyjœæ z programu.";

			String title = _title;
			title += " (" + getFileName(file->getFilename()) + ")";

			int r = MessageBoxW(0, msg.w_str(), title.w_str(), MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON2);
			switch (r) {
				case IDIGNORE:
					return iInterface::fail;
				case IDABORT:
					throw e;
			}
			return iInterface::retry;
		}

		Result handleMessageErrors(FileBase* file, DTException* e, const StringRef& title) {
			String msg;
			bool confirm = true;
			switch (e->errorCode) {
				case DT::errBadFormat: msg = L"Z³y format pliku"; break;
				case DT::errBadParameter: msg = L"Z³y parametr"; break;
				case DT::errBadType: msg = L"Z³y typ"; break;
				case DT::errBadVersion: msg = L"Nieobs³ugiwana wersja pliku - "; msg += file->getVersion().getString(); break;
				case DT::errFileError: msg = L"B³¹d systemu plików - "; msg += inttostr(e->errorCode & ~DT::errFileError); break;
				//case DT::errFileNotFound: msg = L"Plik nie zosta³ znaleziony"; break;
				case DT::errWriteError: msg = L"B³¹d zapisu"; break;
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

		virtual Result handleFailedLoad(FileBase* file, DTException* e, int retry) {
			if (e->errorCode == DT::errNotAuthenticated) {
				return this->handleNotAuthenticated(file, retry);
			} else if (file->getClass() >= FileBin::staticClassInfo()) {
				FileBin* fb = file->castObject<FileBin>();
				if (e->errorCode == DT::errBadFormat && fb->makeBackups) {
					return this->handleRestoreBackup(fb, e, retry);
				}
			}
			return handleMessageErrors(file, e, "B³¹d podczas wczytywania pliku");
		}

		virtual Result handleFailedSave(FileBase* file, DTException* e, int retry) {
			if (e->errorCode == DT::errNotAuthenticated) {
				return this->handleNotAuthenticated(file, retry);
			} else {
			}
			return handleMessageErrors(file, e, "B³¹d podczas zapisywania pliku");
		}

		virtual Result handleFailedAppend(FileBase* file, DTException* e, int retry) {
			if (e->errorCode == DT::errNotAuthenticated) {
				return this->handleNotAuthenticated(file, retry);
			} else if (file->getClass() >= FileBin::staticClassInfo()) {
				FileBin* fb = file->castObject<FileBin>();
				if (e->errorCode == DT::errBadFormat && fb->makeBackups) {
					Result res = this->handleRestoreBackup(fb, e, retry);
					if (res == iInterface::failQuiet) {
						return res;
					}
				}
			}
			return handleMessageErrors(file, e, "B³¹d podczas dopisywania do pliku");
		}


		virtual Result handleNotAuthenticated(FileBase* file, int retry) {
			return iInterface::fail;
		}

		virtual Result handleRestoreBackup(FileBin* file, DTException* e, int retry) {
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

		virtual Result handleNotAuthenticated(FileBase* file, int retry) {
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