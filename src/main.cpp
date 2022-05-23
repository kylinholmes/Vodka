#include "Config.h"
#include "Context.h"
#include "File.h"
#include "Master.h"
#include "Route.h"
#include "form.h"
#include "framework.h"

#include "Render.h"
#include <iostream>
#include "log.h"

using namespace std;




int main() {
  Debug("This is Debug\n");
  Engine e;
  
  auto r = Route::GetInstance();
  r->Use(
      "/",
      [](Context &ctx) {
        ctx.SetHeader("Server", "Kylin's Demo");
        ctx.SetHeader("Inspired-By", "Fawkes");
        ctx.SetHeader("Thanks", "Sheey");
      }
      // , [](Context &ctx){
      //   ctx.Next();
      //   Info("Resuest Path: {}\n", ctx.Path());
      // }
      
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
  e.Run();

  return 0;
}
