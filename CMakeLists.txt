# Vodka CmakeLists
cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE "Debug")
set(VODKA_VERSION 0.14)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG_MODE)
endif()
add_definitions(-DVODKA_VERSION="${VODKA_VERSION}")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_CMAKE}
  CACHE STRING "Vcpkg toolchain file")
set(CMAKE_EXPORT_COMPILE_COMMANDS On)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-terminate -Wno-return-local-addr")
file(GLOB CORE "src/Core/*.cpp")
file(GLOB FRAME_WORK "src/Frame_work/*.cpp")
file(GLOB RENDER "src/Render/*.cpp")

set(
    SOURCE 
    ${CORE}
    ${FRAME_WORK}
    ${RENDER}
    src/main.cpp
)

project( Vokda )
add_executable( ${PROJECT_NAME}  ${SOURCE})

include_directories(
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Frame_work
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Render
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/hiberlite/include
)

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
    uring
)
add_subdirectory(src/hiberlite)
target_link_libraries(${PROJECT_NAME} PUBLIC hiberlite)

# Denpendencies
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)


find_path(TINYTOML_INCLUDE_DIRS "toml/toml.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${TINYTOML_INCLUDE_DIRS})