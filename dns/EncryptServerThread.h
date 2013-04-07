// EncryptServerThread.h
#ifndef __ENCRYPTSERVERTHREAD_H
#define __ENCRYPTSERVERTHREAD_H

#include "Thread.h"
#include "TcpListener.h"

namespace PracticaCaso
{
	class EncryptServerThread: public Thread {
		private:
			TcpClient* client;
			void run();
		public:
			EncryptServerThread(TcpClient* c): client(c) {}
			~EncryptServerThread();
	};
};
#endif
