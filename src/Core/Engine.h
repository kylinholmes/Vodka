#ifndef _MASTER_H_
#define _MASTER_H_

#include <liburing.h>

#include "Worker.h"

const int WORKER_MAX_COUNT = 32;
struct EngineOption{
	int listen_port;
	int worker_count;
};
using Option = EngineOption;

class Engine {
	  public:
	  	Engine():listen_port(8080),worker_count(8){};
		void Run();
		void End();
		void SetOption(EngineOption);

	  private:
		void Loop();
		int	 RebootWorker(pid_t pid);

	  private:
		int				   listen_port;
		int				   worker_count;

		int				   server_sock;
		Uring			   uring;
		EventPackage	   event_for_accept;
		struct sockaddr_in client_addr;
		socklen_t		   client_addr_len = sizeof(client_addr);
		ProcessHandle	   childHandle[WORKER_MAX_COUNT];
};
using  Master = Engine;
#endif
