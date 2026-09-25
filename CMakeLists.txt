cmake_minimum_required(VERSION 3.22)
project(RobloxStudioMobileCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(rsm_core STATIC
    src/core/instance.cpp
    src/core/data_model.cpp
    src/core/base_part.cpp
)

target_include_directories(rsm_core PUBLIC src)

if (MSVC)
    target_compile_options(rsm_core PRIVATE /W4)
else()
    target_compile_options(rsm_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(rsm_smoke tests/smoke_test.cpp)
target_link_libraries(rsm_smoke PRIVATE rsm_core)
