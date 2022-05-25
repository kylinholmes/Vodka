# Vodka

## prerequiste
there are some denpendencies, before build Vodka, you should install them.

use CMake for managing project
**try yourself** to install it.

use vcpkg to manage C/C++ packages.
[RTFM](https://github.com/microsoft/vcpkg)

```bash
vcpkg install nlohmann-json fmt
```

set a env variable 
```bash
export VCPKG_CMAKE=/path/to/vcpkg.cmake
# like:
# VCPKG_CMAKE=/root/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Vodka also use a submodule as 3th-p library, after clone the repo, add submodule into you dir.
> submodule is a sqlite ORM lib, you can pass this step, and remove `#include ` modify `CMakeLists.txt`
```bash
git submodule init
git submodule update
```


## build
```bash
mkdir build
cmake -B build -S .
cmake --build build/ -j $(nproc)
```

## example
example is in src/main.cpp
![TQ5OCLXQYSGG9L0FTA59(S3](https://user-images.githubusercontent.com/45586871/170319058-2dacdf2e-889b-407a-9bd5-5dc684d4f0b2.png)
![image](https://user-images.githubusercontent.com/45586871/170319115-45b2ebf0-2f6d-4444-95de-af5654cb402d.png)

## API ref
```cpp
using HandlerFunc = std::function<void(Context&)>;

Route::Use(std::string pattern, HandlerFunc handler);

Route::Bind(std::string pattern, HandlerFunc handler);

```
