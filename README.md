# Vodka
Vodka is a light fast and modern web framework for C++.

name comes from another web framework `Gin` in Go. In the setting of 名探偵コナン, gin and vodka are partners.

## Features
- ✔ Fast IO Model `uring` 
- ✔ Easy to use API 
- ✔ File Optimized
- ✔ Light Weight, memory efficient
- ✔ 3rd-lib support json and sqlite ORM
- ✔ No side effect Debug Mode
- ❌ Full HTTP Protocol support
- ❌ Cross-platform support
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
vcpkg install nlohmann-json fmt tinytoml
```
- nolohmann-json: a json library for C++
- fmt: a fast and elegent output library
- tinytoml: a tiny TOML parser for parsing `config.toml` file.

set a env variable 
```bash
export VCPKG_CMAKE=/path/to/vcpkg.cmake
# like:
# VCPKG_CMAKE=/root/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Vodka also use a submodule as 3rd-p library, after clone the repo, add submodule into you dir.
> submodule is a sqlite ORM lib, you can pass this step, and remove `#include "hiberlite"`, modify `CMakeLists.txt`
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

1. it will init a db, create User table. 
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
void Route::Bind(std::string pattern, HandlerFunc handler, std::string Method="");
HandlerFunc SendFile(std::string path);

// Engine.h
void Engine::Run();
void Engine::SetOption(EngineOption opt);
void Engine::Config(std::string_view config);

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
void Context::Json(nolohmann::json j);

// Control Flow
void Context::Next();
void Context::Abort();
void Context::AddHandlerFunc(HandlerFunc handler);
void Context::AddResponseHandler(HandlerFunc handler);
```
## Performance
### Memory Use

![image](https://user-images.githubusercontent.com/45586871/170446926-20aa81d0-3006-463b-b4b3-ccf3b7a5c9c2.png)

### Pressure 

![image](https://user-images.githubusercontent.com/45586871/170447008-d3248a71-01ef-418a-a83d-c792ced2dd8b.png)

Vodka got a almost 7w req/s. while Nginx is about 1.4w req/s in the same machine.
It's 5 times that of nginx.

## Output

![image](https://user-images.githubusercontent.com/45586871/170821461-f754fa9f-4654-4c28-bc68-88270bd9cfcb.png)
ez to use API for output

```cpp
template <typename... Args> inline void Debug(std::string_view format, Args... args);
template <typename... Args> inline void Info (std::string_view format, Args... args);
template <typename... Args> inline void Warn (std::string_view format, Args... args);
template <typename... Args> inline void Error(std::string_view format, Args... args);
```

When build in `Release` version, all `Debug` info will magiclly disappear

![~0 6N21GCZA2%CEJ AQ5VIA](https://user-images.githubusercontent.com/45586871/170821840-220dccf1-126a-4d42-a941-46535c363cca.png)


