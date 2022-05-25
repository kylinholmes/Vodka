#include "Engine.h"
#include "Framework.h"


#include "log.h"
#include <iostream>
#include <string>
#include "hiberlite.h"
using namespace std;

struct User{
  string name;
  string password;
  friend class hiberlite::access;
  template <class Archive> void hibernate(Archive& ar) {
				ar& HIBERLITE_NVP(name);
				ar& HIBERLITE_NVP(password);
		}
};
HIBERLITE_EXPORT_CLASS(User)

int main() {
  using namespace hiberlite;
	hiberlite::Database db("test.db");
  db.registerBeanClass<User>();
  db.dropModel();
  db.createModel();
  
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
       //, [](Context &ctx){
       //  ctx.Next();
       //  Info("Resuest Path: {}\n", ctx.Path());
       //}
      );
  
  r->Static("assets");
  

  r->Bind("/login", [&](Context &ctx){
    Form form(ctx.Body());
    auto username = form.Get("name");
    auto password = form.Get("pass");
    
    Info("Login Username: {} Password: {}\n", username, password);
    json j;
    auto user = db.getAllBeans<User>();
    for(auto i :user){
      if(i->name == username && i->password == password){
        j["status"] = "success";
        ctx.SetHeader("Set-Cookie", "12345");
        ctx.Json(j);
        return;
      }
    }
    
    j["status"] = "failed";
    ctx.Json(j);
    
  });
  
  r->Bind("/register", [&](Context &ctx){
    Form form(ctx.Body());
    auto username = form.Get("name");
    auto password = form.Get("pass");
    Info("Regisiter {} {}\n", username, password);
    User u;
    auto p = db.copyBean(u);
    p->name = username;
    p->password = password;
    json j;
    j["status"] = "success";

    ctx.Json(j);
  });
  
  
  r->Bind("/query", [&](Context &ctx){
    // if(ctx.Header("Cookie") != "12345"){
    //   json j;
    //   j["status"] = "failed";
    //   ctx.SetBody(to_string(j));
    //   ctx.SetHeader("Content-Type", "application/json");

    //   return;
    // }
    
    // Info("List All User\n");
    // auto user = db.getAllBeans<User>();
    // for(auto i :user){ 
    //   Info("User: {} {}\n", i->name, i->password);
    // }
  });

  Engine e;
  e.SetOption({
    .listen_port=8080, 
    .worker_count=8,
    .host = "localhost"
    });

  e.Run();

  return 0;
}
