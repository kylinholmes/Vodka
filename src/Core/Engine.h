#ifndef _MASTER_H_
#define _MASTER_H_

#include <liburing.h>
#include <string_view>

#include "Worker.h"
#include "toml/toml.h"

const int WORKER_MAX_COUNT = 32;

struct EngineOption{
	int listen_port;
	int worker_count;
	std::string host;
};
using Option = EngineOption;

class Engine {
	  public:
	  	Engine():opt(){};
		void Run();
		void End();
		Engine& SetOption(EngineOption);
		Engine& Config(std::string_view);

	  private:
		void Loop();
		int	 RebootWorker(pid_t pid);

	  private:
	  	EngineOption	   opt{.listen_port=8080, .worker_count=8, .host="localhost"};

		int				   server_sock;
		Uring			   uring;
		EventPackage	   event_for_accept;
		struct sockaddr_in client_addr;
		socklen_t		   client_addr_len = sizeof(client_addr);
		ProcessHandle	   childHandle[WORKER_MAX_COUNT];
};
using  Master = Engine;
#endif
