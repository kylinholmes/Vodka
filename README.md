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

## API ref
```cpp
using HandlerFunc = std::function<void(Context&)>;

Route::Use(std::string pattern, HandlerFunc handler);

Route::Bind(std::string pattern, HandlerFunc handler);

```