#include "Render.h"

struct User {
  std::string name;
  std::string password;
};

void to_json(nlohmann::json& j, const User u){
	j = nlohmann::json {
		{"name", u.name},
		{"password", u.password}
	};
}
void from_json(const nlohmann::json& j, User& u){
	j.at("name").get_to(u.name);
	j.at("password").get_to(u.password);
}