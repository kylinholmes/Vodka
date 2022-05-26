#include "Engine.h"
#include "Framework.h"


#include "Route.h"
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

void SetDefaultHeader(){
  auto r = Route::GetInstance();
  r->Use(
      [](Context &ctx) {
        ctx.SetHeader("Server", "Kylin's Demo");
        ctx.SetHeader("Inspired-By", "Fawkes");
        ctx.SetHeader("Thanks", "Sheey");
      	ctx.SetHeader("Connection", "Keep-Alive");
	    }
      );  
  r->Static("assets");
}

int main() {
  using namespace hiberlite;
	hiberlite::Database db("test.db");
  db.registerBeanClass<User>();
  
  // drop and crate new one
  db.dropModel();
  db.createModel();
  
  Debug("This is Debug\n");
  
  SetDefaultHeader();

  auto r = Route::GetInstance();
  r->Use("/", [](Context &ctx) {
    Info("{}\n", ctx.Path());
    ctx.Next();
  });

  r->Bind("/login", [&](Context &ctx){
    Form form(ctx.Body());
    auto username = form.Get("name");
    auto password = form.Get("pass");
    
    Info("Login Username: {} Password: {}\n", username, password);
    json j;
    auto user = db.getAllBeans<User>();
    for(auto &i :user){
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
    Info("{}\n",ctx._req.headers);
    json j;
    if(ctx.Header("Cookie") != "12345"){
      j["status"] = "failed";
      j["reason"] = "no cookie";
      ctx.Json(j);
      return;
    }
    auto user = db.getAllBeans<User>();
    j["status"] = "success";
    Info("{}\n", user.size());
    j["number"] = user.size();
    json::array_t a;
    
    for(auto &i :user){ 
      a.push_back({ {"User",i->name}, {"Password", i->password} });
      Info("User: {} ,Password: {}\n", i->name, i->password);

    }
    j["data"] = a;
    ctx.Json(j);
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
