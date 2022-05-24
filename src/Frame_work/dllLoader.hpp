#ifndef DLLLOADER_HPP
#define DLLLOADER_HPP

#include <exception>
#include <stdexcept>

#ifdef _WIN32
    #include <Windows.h>

    struct dllLoader{
        HMODULE dll;
        dllLoader(const char* dllName) {
            dll = LoadLibraryA(dllName);
            if (dll == nullptr){
                throw std::runtime_error("LoadLibrary error");
            }
        }
        FARPROC getFunc(const char* procName) {
            auto mouseHookFunc = GetProcAddress(dll, procName);
            if(mouseHookFunc == nullptr) {
                throw std::runtime_error("Load Func error");
            }
            return mouseHookFunc;
        }
        ~dllLoader() {
            FreeLibrary(dll);
        }
    };
#endif

#ifdef __linux__
    #include <dlfcn.h>
    struct dllLoader{
        void* handle;
        char *error;
        
        dllLoader(const char* dllName) {
            handle = dlopen(dllName, RTLD_LAZY);
            if (handle == nullptr){
                fprintf (stderr, "%s\n", dlerror());
                throw std::runtime_error("Load Library error");
            }
        }
        void* getFunc(const char* procName) {
            auto mouseHookFunc = dlsym(handle, procName);
            if(mouseHookFunc == nullptr) {
                fprintf (stderr, "%s\n", dlerror());
                throw std::runtime_error("Load Func error");
            }
            return mouseHookFunc;
        }
        ~dllLoader() {
            dlclose(handle);
        }
    };
#endif

#endif