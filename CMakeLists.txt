cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_CMAKE}
  CACHE STRING "Vcpkg toolchain file")
set(CMAKE_EXPORT_COMPILE_COMMANDS On)

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
)

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
    uring
)

# Denpendencies
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
