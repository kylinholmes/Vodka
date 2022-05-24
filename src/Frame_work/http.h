#pragma once

#include <cstring>
#include <iostream>
#include <list>
#include <string>
#include <string_view>
#include <unordered_map>
#include "EventPackage.h"

namespace HTTP {

struct URI {
    std::string_view path;
    std::unordered_map<std::string_view, std::string_view> params;
    URI() = default;
    URI(std::string_view uri);
    std::pair<int, int> match(const char* s, size_t len);
};

struct Request {
    Request() = default;
    Request(const char* req);

    std::string_view method, __url__, version;
    std::unordered_map<std::string_view, std::string_view> headers;
    URI url;
    std::string_view body;

   private:
    int match_key(const char* s, int offset = 0);
    int match_value(const char* s, int offset = 0);
};

struct Response {
    Response()
        : version("HTTP/1.1"), status_code("200"), status_message("OK"){};
    // TODO
    bool use_event = false;  // data is write to event->body
    
    std::string version;
    std::string status_code;
    std::string status_message;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};  // struct Response


}  // namespace HTTP