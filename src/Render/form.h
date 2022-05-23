#pragma once
#include <cstddef>
#include <string>
#include <string_view>
#include <list>

struct Form{
    using String = std::string_view;
    Form():data(){};
    Form(String s);
    String Get(String key);
    void Set(String key, String value);
    std::list<std::pair<String,String>> data;

    private:
    std::pair<size_t, size_t> match(const char* data, size_t len);
};