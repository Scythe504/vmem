cmake_minimum_required(VERSION 3.16)
project(vmem)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
include_directories(include)

add_executable(vmem
  src/main.c
  src/page.c
  src/pmem.c
  src/stats.c
  src/tlb.c
  src/translate.c
)

target_compile_options(vmem PRIVATE -g)