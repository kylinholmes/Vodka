#include "Context.h"
#include <fmt/format.h>
#include <sys/types.h>

std::string_view Context::Path() { return req.url.path; }

std::string_view Context::Method() { return req.method; }

std::string_view Context::Params(std::string_view key) {
  return req.url.params[key];
}

std::string_view Context::Header(std::string_view key) {
  return req.headers[key];
}

std::string_view Context::Body() { 
  return req.body; 
}
Context& Context::SetHeader(std::string key, std::string value) {
  resp.headers.push_back(std::make_pair(key, value));
  return *this;
}

void Context::SetStatus(std::string code, std::string msg) {
  resp.status_code = code;
  resp.status_message = msg;
}

void Context::SetStatus(u_int32_t code, std::string msg) {
  resp.status_code = std::to_string(code);
  resp.status_message = msg;
}

Context&  Context::SetBody(std::string_view value) { 
    resp.body = value; 
    return *this;
}

Context&  Context::SetBody(char *value, size_t len) {
  event->ioves[1].iov_len = len;
  event->ioves[1].iov_base = value;
  resp.use_event = true;
  return *this;
}

void Context::Json(json j){
  resp.body = to_string(j);
  SetHeader("Content-Type", "application/json");
  SetHeader("Content-Length", fmt::to_string(resp.body.size()));
  OK();
}

void Context::OK(){
  SetStatus("200", "OK");
}

void Context::Run() {
  handler_iter = handler_list.begin();
  this->Next();
  // _response_handler(*this);
}

void Context::Next() {
  for (; handler_iter != handler_list.end();) {
    if (is_abort)
      return;
    (*handler_iter++)(*this);
  }
}

void Context::Abort() { 
  is_abort = true; 
}

void Context::AddHandlerFunc(HandlerFunc func) {
  handler_list.push_back(func);
}

void Context::AddHandlerFunc(std::list<HandlerFunc> funcs) {
  handler_list.splice(handler_list.end(), funcs);
}

size_t Context::HandlerSize() { 
  return handler_list.size(); 
}
void Context::WriteEvent() {
  std::string tmp = (resp.version + " " + resp.status_code + " " +
                     resp.status_message + "\r\n");

  for (auto &h : resp.headers) {
    tmp += (h.first + ": " + h.second + "\r\n");
  }
  tmp += "\r\n";

  memcpy(event->head, tmp.c_str(), tmp.length());
  event->ioves[0].iov_base = &(event->head);
  event->ioves[0].iov_len = tmp.length();

  if (!resp.use_event) {
    memcpy(event->body, resp.body.c_str(), resp.body.length());
    event->ioves[1].iov_len = resp.body.length();
    event->ioves[1].iov_base = &(event->body);
  }

  event->iovec_cnt = 2;
  event->m_eventType = EVENT_TYPE_WRITEV;
}