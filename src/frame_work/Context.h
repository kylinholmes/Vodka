#pragma once
#include <assert.h>
#include <functional>
#include <string_view>
#include <sys/types.h>
#include "EventPackage.h"
#include "http.h"
#include "utils.h"

struct Context {
    using Request = HTTP::Request;
    using Response = HTTP::Response;
    using HandlerFunc = std::function<void(Context& ctx)>;

    Context() : Context(nullptr, nullptr){};
    Context(const char* s, EventPackage *e)
        : req(s), resp(), event(e), handler_list(), handler_iter(){};

    
    std::string_view IP();  // TODO
    std::string_view Path();
    std::string_view Method();
    std::string_view Params(std::string_view key);
    std::string_view Header(std::string_view key);
    std::string_view Body();

    
    void SetStatus(std::string code, std::string msg);
    void SetStatus(u_int32_t code, std::string msg);
    Context& SetHeader(std::string key, std::string value);
    Context& SetBody(std::string_view value);
    Context& SetBody(char* value, size_t len);
    void Json(json j);
    void OK();
    
    void Run();
    void Next();
    void Abort();
    void WriteEvent();
    void AddHandlerFunc(HandlerFunc func);
    void AddHandlerFunc(std::list<HandlerFunc> funcs);
    
    size_t HandlerSize();

    Request req;
    Response resp;

    EventPackage *event;
    std::list<HandlerFunc> handler_list;
    
    std::list<HandlerFunc>::iterator handler_iter;
    std::string src_ip;
    bool is_abort = false;
};

using HandlerFunc = std::function<void(Context& ctx)>;
