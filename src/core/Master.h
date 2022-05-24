#ifndef _MASTER_H_
#define _MASTER_H_

#include <liburing.h>

#include "Worker.h"

const int WORKER_PORCESS_COUNT = 8;

class Master {
	  public:
	  	Master():listen_port(8080){};
		void Run();
		void End();

	  private:
		void Loop();
		int	 RebootWorker(pid_t pid);

	  private:
		int				   server_sock;
		int				   listen_port;
		Uring			   uring;
		EventPackage	   event_for_accept;
		struct sockaddr_in client_addr;
		socklen_t		   client_addr_len = sizeof(client_addr);
		ProcessHandle	   childHandle[WORKER_PORCESS_COUNT];
};
using Engine = Master;
#endif
