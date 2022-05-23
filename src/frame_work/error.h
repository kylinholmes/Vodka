#pragma once

#include "Context.h"
// #include <exception>
// #include <stdexcept>

void NotFound(Context &ctx);

enum struct Result{
    Success,
    FileNotFound,
    NoResponseHandler,
    OpenFileError,
    InvalidFileType,
    ServerError
};

// struct FileNotFound : std::exception {
//   std::string _file;
//   FileNotFound() : _file() {}
//   FileNotFound(std::string_view file) : _file(file) {}
//   std::string what() noexcept { return "File Not " + _file + " Found"; }
//   static void Handler(Context &ctx);
// };
// struct NoResponseHandler : std::exception {
//   const char *what() const noexcept override { return "No Request Handler"; }
//   static void Handler(Context &ctx);
// };

// struct ServerError : std::exception {
//   const char *what() const noexcept override { return "Server Error"; }
//   static void Handler(Context &ctx);
// };

// struct OpenFileError : std::exception {
//   std::string _file;
//   OpenFileError() : _file() {}
//   OpenFileError(std::string_view file) : _file(file) {}
//   const char *what() const noexcept override { return ("Open File Error"); }
//   static void Handler(Context &ctx);
// };