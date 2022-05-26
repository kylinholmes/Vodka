# Vodka
Vodka is a light fast and modern web framework for C++.
name comes from another web framework `Gin` in Go. In the setting of 名探偵コナン, gin and vodka are partners.
## Features
- [x] Fast IO Model `uring`
- [x] Easy to use API 
- [x] File Optimized
- [x] Light Weight, memory efficient
- [x] 3rd-lib support json and sqlite ORM
- [ ] TCP Connection Pool
- [ ] Full HTTP Protocol support
## Environments
Linux-kernel 5.16.9_x86_64

## Prerequiste
there are some denpendencies, before build Vodka, you should install them.

use **liburing** [link](https://github.com/axboe/liburing)
```bash
make install
```

use **vcpkg** to manage C/C++ packages.
[RTFM](https://github.com/microsoft/vcpkg)

```bash
vcpkg install nlohmann-json fmt
```
- nolohmann-json: a json library for C++
- fmt: a fast and elegent output library

set a env variable 
```bash
export VCPKG_CMAKE=/path/to/vcpkg.cmake
# like:
# VCPKG_CMAKE=/root/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Vodka also use a submodule as 3rd-p library, after clone the repo, add submodule into you dir.
> submodule is a sqlite ORM lib, you can pass this step, and remove `#include ` modify `CMakeLists.txt`
```bash
git submodule init
git submodule update
```

use **CMake** for managing project, **try yourself** to install it.

## Build
```bash
mkdir build
cmake -B build -S .
cmake --build build/ -j $(nproc)
```

## Example
example is in src/main.cpp
![TQ5OCLXQYSGG9L0FTA59(S3](https://user-images.githubusercontent.com/45586871/170319058-2dacdf2e-889b-407a-9bd5-5dc684d4f0b2.png)
![image](https://user-images.githubusercontent.com/45586871/170319115-45b2ebf0-2f6d-4444-95de-af5654cb402d.png)

1. it will init a db, storage User table. 
2. register static file, auto mapping to route.
3. register following pattern:
    /register 
    /login      : after login success, it will set cookie as 12345
    /query      : when sending to /query with a given cookie(12345), it will return the all user info.


## API ref
```cpp
using HandlerFunc = std::function<void(Context&)>;

// Route.h
void Route::Use(std::string pattern, HandlerFunc handler);
void Route::Bind(std::string pattern, HandlerFunc handler);
HandlerFunc SendFile(std::string path);

// Engine.h
void Engine::Run();
void Engine::SetOption(EngineOption opt);

// Context.h
// Request Info
std::string_view Context::Path();
std::string_view Context::Method();
std::string_view Context::Params(std::string_view key);
std::string_view Context::Header(std::string_view key);
std::string_view Context::Body();
// Setting Response 
void Context::SetHeader(std::string key, std::string value);
void Context::SetBody(std::string body);
void Context::SetBody(char* body, size_t size);
void Context::Json(json j);

// Control Flow
void Context::Next();
void Context::Abort();
void Context::AddHandlerFunc(HandlerFunc handler);
void Context::AddResponseHandler(HandlerFunc handler);
```
## Performance Testing

Vodka get a almost 7w req/s. while Nginx is about 1.4w req/s in the same machine.
It is 5 times that of nginx.