#include "form.h"
#include <cstddef>

// name=kylin&passwd=1234
std::pair<size_t, size_t> Form::match(const char *data, size_t len) {
  size_t i = 0;
  for (; data[i] != '='; i++)
    ;
  size_t j = i + 1;
  for (; true; j++) {
    if (data[j] == '&')
      break;
    if (j >= len)
      break;
    if (data[j] == '\r')
      break;
    if (data[j] == ' ')
      break;
  }
  return std::make_pair(i, j);
}

Form::Form(std::string_view s) {
  size_t pos = 0;
  for (; pos < s.length();) {
    auto [k, v] = match(s.data() + pos, s.length() - pos);
    auto key = s.substr(pos, k);
    auto value = s.substr(pos + k + 1, v - k - 1);
    data.push_back(std::make_pair(key, value));
    pos += v + 1;
  }
}

std::string_view Form::Get(std::string_view key) {
  for (auto &[k, v] : data) {
    if (k == key) {
      return v;
    }
  }
  return {};
}
void Form::Set(std::string_view Key, std::string_view Value) {
  for (auto &[k, v] : data) {
    if (k == Key) {
      v = Value;
      return;
    }
  }
  data.push_back(std::make_pair(Key, Value));
}

std::string to_string(Form form){
  std::string result;
  for (auto &[k, v] : form.data) {
    result += k;
    result += "=";
    result += v;
    result += "&";
  }
  if (result.length() > 0) {
    result.pop_back();
  }
  return result;
}