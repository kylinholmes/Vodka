#pragma once
#include "Context.h"
#include "File.h"
#include "error.h"
#include <cstddef>
#include <dirent.h>
#include <functional>
#include <list>

#include <cstring>
#include <memory>
#include <string>
using HandlerFunc = std::function<void(Context& ctx)>;

std::list<std::string_view>
split(std::string_view str, char delimiter = '/', size_t offset = 1);
std::string_view operator-(std::string_view path, std::string_view prefix);

void GetAllFiles(std::string									 BasePath,
				 std::list<std::pair<std::string, std::string>>& list);

struct RouteNode
{
	RouteNode*			   Parent;
	std::list<RouteNode*>  Children;
	std::string			   Pattern;
	std::list<HandlerFunc> Handlers;
	bool				   accurate = true;
	HandlerFunc			   ResponseHandler[10];

	std::string_view prefix;

	RouteNode* GetChild(std::string_view pattern, bool create = true);
	void	   AddHandler(HandlerFunc handler);
	void	   AddHandler(std::list<HandlerFunc> handlers);
	void	   AddResponseHandler(std::string method, HandlerFunc handler);
};

HandlerFunc SendFile(std::string file, bool auto_add = true, size_t offset = 0);

constexpr size_t MethodIndex(std::string_view Method);

struct REST
{
	REST()
		: Any(nullptr),
		  Get(nullptr),
		  Post(nullptr),
		  Put(nullptr),
		  Delete(nullptr),
		  Patch(nullptr),
		  Head(nullptr),
		  Options(nullptr),
		  Connect(nullptr),
		  Trace(nullptr)
	{
	}

	HandlerFunc Any;
	HandlerFunc Get;
	HandlerFunc Head;
	HandlerFunc Post;
	HandlerFunc Put;
	HandlerFunc Delete;
	HandlerFunc Patch;
	HandlerFunc Options;
	HandlerFunc Connect;
	HandlerFunc Trace;

};

struct Route
{
	RouteNode*					  Root;
	static Route* GetInstance()
	{
		static Route r;
		return &r;
	}

	RouteNode* GetNode(std::string_view pattern);

	void WrapContext(Context& ctx);

	void Use(HandlerFunc func);
	void Use(std::list<HandlerFunc> func);

	template <typename... Func> void Use(std::string_view pattern, Func... func)
	{
		auto node = GetNode(pattern);
		if (sizeof...(func) == 0)
			return;
		node->AddHandler(std::list<HandlerFunc>{func...});
	}

	void
	Bind(std::string_view pattern, HandlerFunc func, std::string Method = "");
	
	// TODO
	void Bind(std::string_view pattern, REST* r);
	void Static(std::string_view dir);

	private:
	Route() : Root(new RouteNode{nullptr, {}, "/", {}, true, {}, {}}){};
};

enum Method {
	ANY,
	GET,
	POST,
	PUT,
	DELETE,
	HEAD,
	OPTIONS,
	CONNECT,
	PATCH,
	TRACE,
};
