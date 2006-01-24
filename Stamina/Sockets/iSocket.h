#ifndef __STAMINA_ISOCKET_H__
#define __STAMINA_ISOCKET_H__

#include <Stamina/Object.h>
#include <Stamina/CriticalSection.h>

#include <boost/signal.hpp>

namespace Stamina {
	class iSocket: public iObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::iSocket, iObject, Version(0,1,0,0));
		/** Connection state */
		enum State {
			stOffline,
			stConnecting,
			stConnected,
			stListen,
			stDisconnecting
		};
	public:
		/** Sends data on a connected socket.
		*/
		virtual void send(char* data, unsigned size) = 0;

		/** Gets connection state.
		*/
		virtual State getState() const {
			LockerCS lock(_critical);
			return _state;
		}
	public:
		/** Signal fires when error occures.
		*/
		boost::signal<void (unsigned)> evtOnError;
		/** Signal fires when data has been received.
		*/
		boost::signal<void (char*, unsigned)> evtOnReceived;
		/** Signal fires when connection has been established.
		*/
		boost::signal<void ()> evtOnConnected;
		/** Signal fires when connection has been closed.
		*/
		boost::signal<void ()> evtOnClosed;
	protected:
		State _state;
		CriticalSection_w32 _critical;
	};
};

#endif	// __STAMINA_ISOCKET_H__