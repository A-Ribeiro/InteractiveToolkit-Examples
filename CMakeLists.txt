cmake_minimum_required (VERSION 3.5.1)

project(InteractiveToolkit-Examples)

# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

ENABLE_LANGUAGE(CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(cmake_tools.cmake)
#include(cmake_include.cmake)

# create CMAKE_BUILD_TYPE for non-windows platform
if( NOT WIN32 AND NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
            FORCE )
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)
endif()

# configure compile vars
if(WIN32)
    #
    # Windows
    #
    if(CMAKE_COMPILER_IS_GNUCXX)
        create_missing_cmake_build_type()
        if (CMAKE_BUILD_TYPE STREQUAL Release)
            add_definitions(-DNDEBUG)
            add_compile_options(-O3 -fno-strict-aliasing)
        endif()
    elseif(MSVC)
        # change the multithread debug DLL to multithread debug static compilation
        # avoid link with another VS DLL...
        foreach(flag
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO

                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
                SET ( ${flag} "${${flag}}" CACHE STRING "" FORCE)
            endif()
            set("${flag}" "${${flag}} /MP")
        endforeach()

        #SET(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /O2")
        #SET(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} /O2")
    endif()
elseif(APPLE)
    #
    # MacOS X, iOS, watchOS, tvOS (since 3.10.3)
    #
    if (CMAKE_BUILD_TYPE STREQUAL Release)
        add_definitions(-DNDEBUG)
        add_compile_options(-O3 -fno-strict-aliasing)
    endif()
    #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -all_load")
    #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -v")
    #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -ObjC")
elseif(UNIX)
    #
    # Linux, BSD, Solaris, Minix
    #
    if (CMAKE_BUILD_TYPE STREQUAL Release)
        add_definitions(-DNDEBUG)
        add_compile_options(-O3 -fno-strict-aliasing)
    elseif( CMAKE_BUILD_TYPE STREQUAL Debug)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -static-libasan")
    endif()
else()
    message(FATAL_ERROR "Cmake not configured for this OS")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(libs/InteractiveToolkit "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit")
find_package(InteractiveToolkit REQUIRED)
include_directories(${INTERACTIVETOOLKIT_INCLUDE_DIRS})
add_compile_options(${INTERACTIVETOOLKIT_COMPILE_OPTIONS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${INTERACTIVETOOLKIT_LINK_OPTIONS}")

add_subdirectory(src/astar-on-roguelike-matrix)
add_subdirectory(src/callback-handling)
add_subdirectory(src/ease-core)
add_subdirectory(src/event-handling)
add_subdirectory(src/find-rsqrt-parameters)
add_subdirectory(src/math-core)
add_subdirectory(src/on-abort-handling)
add_subdirectory(src/pattern-match)
add_subdirectory(src/property)
add_subdirectory(src/random)
add_subdirectory(src/sorting)

add_subdirectory(src/ipc-queue)
add_subdirectory(src/ipc-shared-buffer)
add_subdirectory(src/platform-process)
add_subdirectory(src/platform-thread-queue)

add_subdirectory(src/socket-udp)


