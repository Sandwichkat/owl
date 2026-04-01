cmake_minimum_required(VERSION 3.5)

project(owl C)

if (CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    add_compile_options("-Wall" "-Wextra")
endif ()

if (NOT WIN32)
    add_library(radiotap radiotap/radiotap.c)
    target_compile_definitions(radiotap PRIVATE -D_BSD_SOURCE -D_DEFAULT_SOURCE -DRADIOTAP_SUPPORT_OVERRIDES)
endif ()

add_subdirectory(src)

option(OWL_BUILD_DAEMON "Build platform daemon executable" ON)
if (WIN32)
    set(OWL_BUILD_DAEMON OFF CACHE BOOL "Build platform daemon executable" FORCE)
endif ()
if (OWL_BUILD_DAEMON)
    add_subdirectory(daemon)
endif ()

option(OWL_BUILD_TESTS "Build unit tests" ON)
if (OWL_BUILD_TESTS)
    if (WIN32)
        # Keep gtest CRT selection aligned with the default /MD runtime of this project on MSVC.
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif ()
    enable_language(CXX)
    option(INSTALL_GTEST "" OFF)
    add_subdirectory(googletest)
    add_subdirectory(tests)
endif ()
