cmake_minimum_required(VERSION 3.0)
project(acquire-driver-common)
enable_testing()

include(cmake/ide.cmake)
include(cmake/git-versioning.cmake)
include(cmake/install-prefix.cmake)
include(cmake/simd.cmake)

# include(cmake/coverage.cmake)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

add_subdirectory(tests)
include(CPack)
