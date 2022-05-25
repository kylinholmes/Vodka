#include "http.h"

HTTP::URI::URI(std::string_view uri) {
    auto pos = 0;
    for (pos; pos < uri.size(); pos++) {
        if (uri[pos] == '?') {
            path = uri.substr(0, pos);
            break;
        }
    }
    if (pos == uri.size()) {
        path = uri;
        return;
    }
    pos += 1;
    // find out all params
    for (; pos < uri.length();) {
        auto [k, v] = match(uri.data() + pos, uri.length() - pos);
        auto key = uri.substr(pos, k);
        auto value = uri.substr(pos + k + 1, v - k - 1);
        params[key] = value;
        pos += v + 1;
    }
}

std::pair<int, int> HTTP::URI::match(const char* s, size_t len) {
    int key_offset = 0;
    for (; s[key_offset] != '='; key_offset++)
        ;

    int value_offset = key_offset + 1;

    for (; true; value_offset++) {
        if (s[value_offset] == '&')
            break;
        if (s[value_offset] == ' ')
            break;
    }

    return {key_offset, value_offset};
}

HTTP::Request::Request(const char* req) {
    auto pos = 0;
    for (int i = 0, cnt = 0; req[i] != '\n'; i++) {
        if (req[i] == ' ') {
            if (cnt == 0) {
                method = std::string_view(req, i);
            }
            if (cnt == 1) {
                __url__ = std::string_view(req + pos + 1, i - pos - 1);
            }
            cnt++;
            pos = i;
        } else if (req[i] == '\r' || req[i] == '\n') {
            version = std::string_view(req + pos + 1, i - pos - 1);
        }
    }
    auto i = method.length() + __url__.length() + version.length() + 4;
    // 3 for two blank and \r\n

    url = URI(__url__);
    while (true) {
        // garrentee that i is always the start of a line, and tmp is
        // the pointer
        auto tmp = req + i;
        auto key_pos = match_key(tmp);
        if (key_pos == -1)
            break;
        auto key = std::string_view(tmp, key_pos);
        auto value_pos = match_value(tmp + key_pos + 1);
        auto value = std::string_view(tmp + key_pos + 1, value_pos);

        i += key_pos + value_pos + 2;
        headers[key] = value;
    }

    body = std::string_view(req + i + 2);
};

int HTTP::Request::match_key(const char* s, int offset) {
    for (; s[offset] != '\r'; offset++) {
        if (s[offset] == ':')
            return offset;
    }
    return -1;
}
int HTTP::Request::match_value(const char* s, int offset) {
    for (; s[offset] != '\r'; offset++)
        ;

    return offset + s[offset + 1] == '\n' ? 0 : offset + 1;
}

