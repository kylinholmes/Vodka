git checkout main
git submodule init
git submodule update
mkdir build
cmake -B build -S .
cmake --build build -j (nproc)
