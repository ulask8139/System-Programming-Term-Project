cmake_minimum_required(VERSION 3.16)
project(sis_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use pkg-config to find libpqxx reliably on Ubuntu/Debian
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

add_executable(sis_app
  main.cpp
  sis_handler.cpp
)

target_include_directories(sis_app PRIVATE . ${PQXX_INCLUDE_DIRS})
target_link_libraries(sis_app PRIVATE ${PQXX_LIBRARIES})
target_compile_options(sis_app PRIVATE ${PQXX_CFLAGS_OTHER})
