#include "File.h"
#include "error.h"
#include <cstring>
#include <string>


std::string GetFileType(std::string_view file) {
  auto pos = file.find_last_of('.');
  if (pos == std::string_view::npos)
    return "text/plain";

  auto suffix = file.substr(pos + 1);
  if (suffix == "html")
    return "text/html";
  if (suffix == "css")
    return "text/css";
  if (suffix == "js")
    return "application/javascript";
  if (suffix == "jpg")
    return "image/jpeg";
  if (suffix == "png")
    return "image/png";
  if (suffix == "ico")
    return "image/x-icon";
  return "text/plain";
}


Result FileManager::AddFile(std::string_view f) {
  auto file = std::string(f);
  
  if (FilesMap.find(file) != FilesMap.end()) {
    return Result::Success;
  }
  int fd = open((file).data(), O_RDONLY);
  if (fd < 0) {
    Error("Open File Error {} :{}\n", file, strerror(errno));
    return Result::OpenFileError;
  }
  struct stat st;
  fstat(fd, &st);
  char *data = (char *)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  FilesMap.insert({file, {data, fd}});
  return Result::Success;
}


std::tuple<char*, int, int, Result> FileManager::GetFile(std::string_view f){
  auto file = std::string(f);
  auto res = FilesMap.find(file);
  if (res == FilesMap.end()) {
    return std::make_tuple(nullptr, 0, 0, Result::FileNotFound);
  }
  
  auto [chars, fd] = (*res).second;
  struct stat st;
  fstat(fd, &st);
  return std::make_tuple(chars, st.st_size, fd, Result::Success);
}