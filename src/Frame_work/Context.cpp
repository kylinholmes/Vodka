#include "Context.h"
#include <fmt/format.h>

std::string_view Context::Path() { return _req.url.path; }

std::string_view Context::Method() { return _req.method; }

std::string_view Context::Params(std::string_view key) {
  return _req.url.params[key];
}

std::string_view Context::Header(std::string_view key) {
  return _req.headers[key];
}

std::string_view Context::Body() { return _req.body; }
void Context::SetHeader(std::string key, std::string value) {
  _res.headers[key] = value;
}
void Context::SetBody(std::string_view value) { _res.body = value; }

void Context::SetBody(char *value, size_t len) {
  event->ioves[1].iov_len = len;
  event->ioves[1].iov_base = value;
  _res.use_event = true;
}
void Context::Json(json j){
  _res.body = to_string(j);
  SetHeader("Content-Type", "application/json");
  SetHeader("Content-Length", fmt::to_string(_res.body.size()));
}

void Context::Run() {
  _iter = _handler_list.begin();
  this->Next();
  _response_handler(*this);
}
void Context::Next() {
  for (; _iter != _handler_list.end();) {
    if (_abort)
      return;
    (*_iter++)(*this);
  }
}

void Context::Abort() { _abort = true; }

void Context::AddHandlerFunc(HandlerFunc func) {
  _handler_list.push_back(func);
}

void Context::AddHandlerFunc(std::list<HandlerFunc> funcs) {
  _handler_list.splice(_handler_list.end(), funcs);
}
void Context::AddResponseHandler(HandlerFunc func) { _response_handler = func; }
size_t Context::HandlerSize() { return _handler_list.size(); }
void Context::WriteEvent() {
  std::string tmp = (_res.version + " " + _res.status_code + " " +
                     _res.status_message + "\r\n");

  for (auto &h : _res.headers) {
    tmp += (h.first + ": " + h.second + "\r\n");
  }
  tmp += "\r\n";

  memcpy(event->head, tmp.c_str(), tmp.length());
  event->ioves[0].iov_base = &(event->head);
  event->ioves[0].iov_len = tmp.length();

  if (!_res.use_event) {
    memcpy(event->body, _res.body.c_str(), _res.body.length());
    event->ioves[1].iov_len = _res.body.length();
    event->ioves[1].iov_base = &(event->body);
  }

  event->iovec_cnt = 2;
  event->m_eventType = EVENT_TYPE_WRITEV;
}