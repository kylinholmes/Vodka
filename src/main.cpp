#include "Context.h"
#include "Engine.h"
#include "Framework.h"
#include "log.h"

#include "Route.h"
#include "log.h"
#include <functional>
#include <iostream>
#include <string>
using namespace std;




int main()
{
	auto &r = *Route::GetInstance();
	r.Use(
		"/",
		[](Context& ctx) {
			ctx.SetHeader("Server", "Kylin's Demo");
			ctx.SetHeader("Inspired-By", "Fawkes");
			ctx.SetHeader("Thanks", "Sheey");
			ctx.SetHeader("Connection", "Keep-Alive");
		},
		[](Context& ctx) {

		});

	
	r.Use("/", [](Context& ctx) {
		ctx.Next();
		Info("{} => {}\n", ctx.Method(), ctx.Path());
	});

	r.Static("assets");

	Engine e;
	// 8080 8 localhost is default value
	e.SetOption({.listen_port = 8080, .worker_count = 8, .host = "localhost"}) 
	 .Config("config.toml")
	 .Run();

	return 0;
}
