#include "error.h"

const char *notfound_html =
    R"(<!DOCTYPE html><html><title>404 Not Found</title><body><center><h1 style="margin-top:40px">404 Not Found</h1></center><h4 style="position: fixed;bottom:0px;text-align:center;left: 46%;bottom: 10%;">By Kylin's Demo;</h4></body></html>)";

void NotFound(Context &ctx) {
  ctx.SetBody(const_cast<char *>(notfound_html), 233);
  ctx._res.status_code = "404";
  ctx._res.status_message = "Not Found";
  ctx.SetHeader("Content-Type", "text/html");
  ctx.SetHeader("Context-Length", "233");
  ctx.Abort();
}
