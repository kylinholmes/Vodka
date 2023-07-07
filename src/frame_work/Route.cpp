#include "Route.h"
#include "Context.h"
#include "error.h"
#include "log.h"
#include <memory>

std::list<std::string_view>
split(std::string_view str, char delimiter, size_t offset) {
		std::list<std::string_view> result;
		auto						i = offset, j = offset;
		for (; i < str.size(); i++) {
				if (str[i] == delimiter && i != j) {
						result.push_back(str.substr(j, i - j));
						j = i + 1;
				}
		}
		if (i != j)
				result.push_back(str.substr(j, i - j));
		return result;
}

std::string_view operator-(std::string_view path, std::string_view prefix) {
		if (prefix.length() > path.length()) {
				throw std::runtime_error("path less than prefix");
		}
		for (int i = 0; i < path.length(); i++) {
				if (path[i] != prefix[i])
						return path.substr(i);
		}
		return {};
}

void GetAllFiles(std::string BasePath,
				 std::list<std::pair<std::string, std::string>>& list) {
		DIR*		   dir;
		struct dirent* ptr;
		if ((dir = opendir(BasePath.data())) == NULL) {
				exit(1);
		}

		while ((ptr = readdir(dir)) != NULL) {
				if (strcmp(ptr->d_name, ".") == 0 ||
					strcmp(ptr->d_name, "..") == 0)
						continue;
				else if (ptr->d_type == DT_REG)
						list.push_back(
							std::make_pair(BasePath + "/", ptr->d_name));
				else if (ptr->d_type == DT_DIR) {
						std::string SubPath = BasePath.data();
						SubPath += "/";
						SubPath += ptr->d_name;
						GetAllFiles(SubPath, list);
				}
		}
		closedir(dir);
}
RouteNode* RouteNode::GetChild(std::string_view pattern, bool create) {
		for (auto iter = Children.begin(); iter != Children.end(); ++iter) {
				if (!(*iter)->accurate || (*iter)->Pattern == pattern) {
						return *iter;
				}
		}

		if (!create)
				return nullptr;
		auto c = new RouteNode{this, {}, std::string(pattern), {}, true, {}, {}};
		// auto c = std::make_shared<RouteNode*> ({this, {}, std::string(pattern), {}, true, {}, {}});

		if (pattern[0] == ':')
				c->accurate = false;

		Children.push_back(c);
		return c;
}

constexpr size_t MethodIndex(std::string_view Method) {
		if (Method.empty())
				return 0;
		if (Method == "GET")
				return 1;
		if (Method == "POST")
				return 2;
		if (Method == "PUT")
				return 3;
		if (Method == "DELETE")
				return 4;
		if (Method == "HEAD")
				return 5;
		if (Method == "OPTIONS")
				return 6;
		if (Method == "CONNECT")
				return 7;
		if (Method == "PATCH")
				return 8;
		if (Method == "TRACE")
				return 9;
		return 0;
}

void RouteNode::AddResponseHandler(std::string method, HandlerFunc handler) {
		auto index= MethodIndex(method);
		ResponseHandler[index] = handler;
}

void RouteNode::AddHandler(HandlerFunc handler) {
		Handlers.push_back(handler);
}

void RouteNode::AddHandler(std::list<HandlerFunc> handlers) {
		Handlers.splice(Handlers.end(), handlers);
}

HandlerFunc SendFile(std::string file, bool auto_add, size_t offset) {
		auto fm = FileManager::GetInstance();
		if (auto_add) {
				auto res = fm->AddFile(file);
				switch (res) {
						case Result::Success:
								Debug("Add File {} Success\n", file);
								break;
						case Result::FileNotFound:
						case Result::OpenFileError:
						default:
								return [=](Context& ctx) {
										ctx.AddHandlerFunc(NotFound);
										Debug("File {} Not Fonund\n", file);
								};
				}
		}
		return [=](Context& ctx) {
				auto fm = FileManager::GetInstance();
				{
						auto [c, l, f, res] = fm->GetFile(file);
						switch (res) {
								case Result::Success:
										Debug("Get  File {} Success\n", file);
										ctx.SetBody(c + offset, l).OK();
										ctx.SetHeader("Content-Type",
													  GetFileType(file));
										ctx.SetHeader("Content-Length",
													  std::to_string(l));
										break;
								case Result::FileNotFound:
								case Result::OpenFileError:
								default:
										ctx.AddHandlerFunc(NotFound);
										Debug("File {} Not Fonund\n", file);
										return;
						}
				}
		};
}

RouteNode* Route::GetNode(std::string_view pattern) {
		auto node = Root;
		for (auto s : split(pattern, '/')) {
				node = node->GetChild(s);
		}
		return node;
}

void Route::WrapContext(Context& ctx) {
		auto node = Root;
		ctx.AddHandlerFunc(Root->Handlers);
		for (auto s : split(ctx.Path())) {
				node = node->GetChild(s);
				if (node == nullptr) {
						Debug("Path Not Exist\n");
						ctx.AddHandlerFunc(NotFound);
						return;
				}
				ctx.AddHandlerFunc(node->Handlers);
		}
		
		auto index = MethodIndex(ctx.Method());
		
		if (node->ResponseHandler[index]) {
			ctx.AddHandlerFunc(node->ResponseHandler[index]);
		}else if (node->ResponseHandler[Method::ANY] != nullptr) {
				ctx.AddHandlerFunc(node->ResponseHandler[Method::ANY]);
				return;
		}
		else {
			Debug("No Handler\n");
			ctx.AddHandlerFunc(NotFound);
		}
}

void Route::Static(std::string_view dir) {
		using namespace std;
		using namespace fmt;
		Debug("Route::Static Mapping Path\n");
		list<pair<string, string>> list;
		GetAllFiles(dir.data(), list);
		auto fm = FileManager::GetInstance();
		for (auto& [path, name] : list) {
				auto prefix = path - dir;
				auto node	= GetNode(prefix.data() + name);
				Debug("Cast: {} => {}\n", prefix.data() + name, path + name);
				fm->AddFile(path + name);
				node->ResponseHandler[Method::GET] =
					SendFile(path + name, false);

				if (name == "index.html") {
						Root->ResponseHandler[Method::GET] =
							SendFile(path + name, false);
				}
		}
}

void Route::Use(HandlerFunc func) {
		Root->AddHandler(func);
}
void Route::Use(std::list<HandlerFunc> func) {
		Root->AddHandler(func);
}

void Route::Bind(std::string_view pattern, HandlerFunc func, std::string Method) {
		auto node= GetNode(pattern);
		auto index= MethodIndex(Method);
		node->ResponseHandler[index] = func;
}