# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

############################################################################
# Detect Raspberry Pi
############################################################################
if(UNIX AND EXISTS "/opt/vc/include/bcm_host.h" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(EXAMPLE_RPI ON CACHE BOOL "Set this if you are buinding for Raspberry PI" )
else()
    set(EXAMPLE_RPI OFF CACHE BOOL "Set this if you are buinding for Raspberry PI" )
endif()
if(EXAMPLE_RPI)
    add_definitions(-DEXAMPLE_RPI)
    # RPI OMX Compilation Flag on 32bit: -DOMX_SKIP64BIT
    if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
        add_definitions(-DOMX_SKIP64BIT)
    endif()
    link_directories(/opt/vc/lib/)
    include_directories(
        /opt/vc/include/interface/vcos/pthreads
        /opt/vc/include/interface/vmcs_host/linux
        /opt/vc/include
    )
    set(EGL_INCLUDE_DIR /opt/vc/include)
    set(EGL_LIBRARY /opt/vc/lib/libbrcmEGL.so)
    set(GLES_INCLUDE_DIR /opt/vc/include)
    set(GLES_LIBRARY /opt/vc/lib/libbrcmGLESv2.so)
    set(RPI_LIBRARIES bcm_host openmaxil vcos vchiq_arm dl)
    message(STATUS "[RPI Detector] Defined Libraries Variables: " )
    message(STATUS "    RPI_LIBRARIES")
    message(STATUS "    GLES_LIBRARY")
    message(STATUS "    EGL_LIBRARY")
endif()
############################################################################
# Detect NEON compatible environment
############################################################################
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(EXAMPLE_NEON ON CACHE BOOL "Turn on NEON optimizations." )
else()
    set(EXAMPLE_NEON OFF CACHE BOOL "Turn on NEON optimizations." )
endif()
if (EXAMPLE_NEON)
    add_definitions(-DEXAMPLE_NEON)
    add_compile_options(-mfpu=neon -finline-functions)
    message(STATUS "[NEON ENABLED]")
endif()
############################################################################
# Detect OpenMP compatible environment
############################################################################
if(NOT CMAKE_GENERATOR STREQUAL Xcode)
    find_package(OpenMP QUIET)
    if (OPENMP_FOUND)
        set(EXAMPLE_OPENMP ON CACHE BOOL "Turn on OPENMP optimizations." )
    else()
        set(EXAMPLE_OPENMP OFF CACHE BOOL "Turn on OPENMP optimizations." )
    endif()
else()
    set(EXAMPLE_OPENMP OFF CACHE BOOL "Turn on OPENMP optimizations." )
endif()
if (EXAMPLE_OPENMP)
    if(CMAKE_GENERATOR STREQUAL Xcode)
        message(FATAL_ERROR "CANNOT ENABLE OPENMP WITH XCODE BUILD.")
    endif()
    find_package(OpenMP REQUIRED)
    add_definitions(-DEXAMPLE_OPENMP)
    include_directories(${OpenMP_C_INCLUDE_DIRS} ${OpenMP_CXX_INCLUDE_DIRS})
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
############################################################################
# Detect SSE2 compatible environment
############################################################################
set(EXAMPLE_SSE_SKIP_SSE41 OFF CACHE BOOL "Skip SSE41 on some special CPUs.")
if(NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(EXAMPLE_SSE2 ON CACHE BOOL "Turn on SSE2 optimizations.")
else()
    set(EXAMPLE_SSE2 OFF CACHE BOOL "Turn on SSE2 optimizations.")
endif()
if (EXAMPLE_SSE2)
    if (EXAMPLE_SSE_SKIP_SSE41)
        list(APPEND ITK_COMPILE_DEFINITIONS EXAMPLE_SSE_SKIP_SSE41)
    endif()
    add_definitions(-DEXAMPLE_SSE2)
    if(NOT MSVC)
        if (EXAMPLE_SSE_SKIP_SSE41)
            add_compile_options(-mmmx -msse -msse2 -msse3 -mfpmath=sse -minline-all-stringops -finline-functions)
        else()
            add_compile_options(-mmmx -msse -msse2 -msse3 -msse4.1 -mfpmath=sse -minline-all-stringops -finline-functions)
        endif()
    endif()
endif()


############################################################################
# Print Result
############################################################################
message(STATUS "")
message(STATUS "All Detection Set:")
message(STATUS "")
message(STATUS "[EXAMPLE_RPI    ${EXAMPLE_RPI}]")
message(STATUS "[EXAMPLE_NEON   ${EXAMPLE_NEON}]")
message(STATUS "[EXAMPLE_OPENMP ${EXAMPLE_OPENMP}]")
message(STATUS "[EXAMPLE_SSE2   ${EXAMPLE_SSE2}]")
message(STATUS "")
