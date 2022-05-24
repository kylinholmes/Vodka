#include "Engine.h"
#include "Framework.h"
#include "Render.h"

#include <iostream>
using namespace std;


int main() {
  Debug("This is Debug\n");

  auto r = Route::GetInstance();
  r->Use(
      "/",
      [](Context &ctx) {
        ctx.SetHeader("Server", "Kylin's Demo");
        ctx.SetHeader("Inspired-By", "Fawkes");
        ctx.SetHeader("Thanks", "Sheey");
      	ctx.SetHeader("Connection", "Keep-Alive");
	    }
       , [](Context &ctx){
         ctx.Next();
         Info("Resuest Path: {}\n", ctx.Path());
       }
      
      );
  
  r->Static("assets");
  

  r->Bind("/login", [&](Context &ctx){
    Form form(ctx.Body());
    auto username = form.Get("name");
    auto password = form.Get("pass");
    
    Info("Username: {} Password: {}\n", username, password);
    
    json j;
    
    if(username == "kylin" && password == "1234"){
        j["status"] = "success";
    }
    else{
        j["status"] = "fail";
    }
    
    ctx.SetBody(to_string(j));
    ctx.SetHeader("Content-Type", "application/json");
  });
  
  r->Bind("/register", [&](Context &cxt){
    
  });
  
  Engine e;
  e.SetOption({
    .listen_port=8080, 
    .worker_count=8,
    .host = "kylinn.cloud"
    });

  e.Run();

  return 0;
}
