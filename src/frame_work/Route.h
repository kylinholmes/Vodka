#pragma once
#include "Context.h"
#include "File.h"
#include "error.h"
#include <cstddef>
#include <functional>
#include <list>
#include <dirent.h>

using HandlerFunc = std::function<void(Context &ctx)>;

std::list<std::string_view> split(std::string_view str, char delimiter = '/',
                                  size_t offset = 1);
std::string_view operator-(std::string_view path, std::string_view prefix);

void GetAllFiles(std::string BasePath, std::list<std::pair<std::string,std::string>>& list);

struct RouteNode {
  RouteNode *Parent;
  std::list<RouteNode *> Children;
  std::string Pattern;
  std::list<HandlerFunc> Handlers;
  bool accurate = true;
  HandlerFunc ResponseHandler;
  // std::string_view Method;
  
  std::string_view prefix;
  /*
      pattern without '/'
  */
  RouteNode *GetChild(std::string_view pattern, bool create = true);
  void AddHandler(HandlerFunc handler);
  void AddHandler(std::list<HandlerFunc> handlers);
};


HandlerFunc SendFile(std::string file, bool auto_add=true, size_t offset=0);

struct Route {
  RouteNode *Root;
  static Route *GetInstance() {
    static Route r;
    return &r;
  }

  RouteNode *GetNode(std::string_view pattern);

  void WarpContext(Context &ctx);

  void Use(HandlerFunc func);
  void Use(std::list<HandlerFunc> func);

  template <typename... Func> void Use(std::string_view pattern, Func... func) {
    auto node = GetNode(pattern);
    if (sizeof...(func) == 0)
      return;
    node->AddHandler(std::list<HandlerFunc>{func...});
  }

  void Bind(std::string_view pattern, HandlerFunc func);
  void Static(std::string_view dir);

private:
  Route()
      : Root(new RouteNode{nullptr, {}, "/", {}, true, {}, {} }){};
};
