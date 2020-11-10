
cmake_minimum_required(VERSION 2.8)
project(pgm)

set(default_build_type "Debug")

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

# If no build type is specified, default to release.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Add G++ Warning on Unix
if(UNIX)
    set(CMAKE_CXX_COMPILER g++)
    find_package(glfw3 REQUIRED) #Expect glfw3 to be installed on your system
endif()

# In Window set directory to precompiled version of glfw3
if(WIN32)
    set(GLFW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/lib_windows/glfw3_win/include")
    include_directories(${GLFW_INCLUDE_DIRS})
    set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/lib_windows/glfw3_win/lib/glfw3.lib")
endif()


include_directories(".")
include_directories("third_party/eigen/")
file(
    GLOB_RECURSE
    source_files
    vcl/*.[ch]pp
    main/*.[ch]pp
    third_party/*.[ch]pp
    third_party/*.[ch]
    scenes/*.[ch]pp
    scenes/*.glsl
    )
add_executable(pgm ${source_files})


if(UNIX)
    target_link_libraries(pgm glfw dl -static-libstdc++)
    target_compile_options(pgm
        PUBLIC -std=c++14 -pipe
        $<$<CONFIG:Debug>:-O0 -ggdb3 -gsplit-dwarf>
        $<$<CONFIG:Release>:-O2 -march=native -flto -fno-fat-lto-objects> # release: 03 is implied, add back 02
        $<$<CONFIG:RelWithDebInfo>:-O2 -march=native -flto -fno-fat-lto-objects -ggdb3 -gsplit-dwarf> # relwithdebinfo: 02 is implied
        PRIVATE -Wall -Wextra -Wpedantic -Wformat=2 -Wswitch-default -Wswitch-enum -Wfloat-equal -Wno-conversion
        -pedantic-errors -Werror=format-security
        -fdiagnostics-color=always
    )
endif()

if(WIN32)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_files}) # Allow to explore source directories as a tree
    target_link_libraries(pgm ${GLFW_LIBRARIES})
endif()
