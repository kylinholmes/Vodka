#pragma once
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string_view>
#include <unordered_map>
#include <exception>
#include <stdexcept>
#include <string>
#include "error.h"
#include <list>
#include "log.h"

std::string GetFileType(std::string_view file);

struct FileManager {
    std::unordered_map<std::string, std::pair<char*, int> > FilesMap;
    
    
    Result AddFile(std::string_view file);
    // void AddFile(std::string_view file);
    std::tuple<char*, int, int, Result> GetFile(std::string_view file);
    // std::tuple<char*, int, int> GetFile(std::string_view file);

    static FileManager* GetInstance() {
        static FileManager fm;
        return &fm;
    }
    ~FileManager(){
        Debug("File Manager unexcepet destruct\n");
    }
};