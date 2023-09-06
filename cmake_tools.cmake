# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

# automatically setup IDE source group. Useful for VSStudio
macro(define_source_group )
    foreach(entry IN ITEMS ${ARGN})
        get_filename_component(dirname "${entry}" DIRECTORY )
        if (dirname)
            string(REPLACE "/" "\\" dirname_replaced ${dirname})
            source_group(${dirname_replaced} FILES ${entry})
        else()
            source_group("" FILES ${entry})
        endif()
    endforeach()
endmacro()

macro(to_unique_list output)
    set(${output})
    foreach(var IN ITEMS ${ARGN})
        if (NOT "${var}" IN_LIST ${output})
            list(APPEND ${output} ${var})
        endif()
    endforeach()
endmacro()

macro(configure_build_flags projectname inputfile outputfile)

    #output
    set(configure_COMPILE_DEFINITIONS "") # generate buildFlags.h
    set(configure_COMPILE_OPTIONS "") # generate opts.cmake
    set(configure_LINK_LIBRARIES "") # generate opts.cmake
    set(configure_LINK_OPTIONS "") # generate opts.cmake
    set(configure_INCLUDE_DIRECTORIES "") # generate opts.cmake

    set(target_COMPILE_DEFINITIONS)
    set(target_COMPILE_OPTIONS)
    set(target_LINK_LIBRARIES)
    set(target_LINK_OPTIONS)
    set(target_INCLUDE_DIRECTORIES)

    if (TARGET ${projectname})
        get_target_property(target_COMPILE_DEFINITIONS ${projectname} COMPILE_DEFINITIONS)
        if ("${target_COMPILE_DEFINITIONS}" STREQUAL "target_COMPILE_DEFINITIONS-NOTFOUND")
            set(target_COMPILE_DEFINITIONS)
        endif()

        get_target_property(target_COMPILE_OPTIONS ${projectname} COMPILE_OPTIONS)
        if ("${target_COMPILE_OPTIONS}" STREQUAL "target_COMPILE_OPTIONS-NOTFOUND")
            set(target_COMPILE_OPTIONS)
        endif()

        get_target_property(target_LINK_LIBRARIES ${projectname} INTERFACE_LINK_LIBRARIES)
        if ("${target_LINK_LIBRARIES}" STREQUAL "target_LINK_LIBRARIES-NOTFOUND")
            set(target_LINK_LIBRARIES)
        endif()

        get_target_property(target_LINK_OPTIONS ${projectname} INTERFACE_LINK_OPTIONS)
        if ("${target_LINK_OPTIONS}" STREQUAL "target_LINK_OPTIONS-NOTFOUND")
            set(target_LINK_OPTIONS)
        endif()

        get_target_property(target_INCLUDE_DIRECTORIES ${projectname} INTERFACE_INCLUDE_DIRECTORIES)
        if ("${target_INCLUDE_DIRECTORIES}" STREQUAL "target_INCLUDE_DIRECTORIES-NOTFOUND")
            set(target_INCLUDE_DIRECTORIES)
        endif()

    endif()

    # get build flags
    #get_directory_property(aux COMPILE_DEFINITIONS)
    #
    # configure_COMPILE_DEFINITIONS
    #
    to_unique_list(aux ${target_COMPILE_DEFINITIONS})
    foreach(var ${aux})
        if("${var}" STREQUAL "ITK_SSE2")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#    ifndef ${var}\n#        define ${var}\n#    endif\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#endif\n")
        elseif("${var}" STREQUAL "ITK_NEON")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#if defined(__arm__) || defined(_M_ARM) || defined(__aarch64__)\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#    ifndef ${var}\n#        define ${var}\n#    endif\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#endif\n")
        elseif(NOT "${var}" STREQUAL "NDEBUG")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#ifndef ${var}\n#    define ${var}\n#endif\n")
        endif()
    endforeach()

    #unset(INTERACTIVETOOLKIT_INCLUDE_DIRS)

    #
    # COMPILE_OPTIONS
    #
    to_unique_list(aux ${target_COMPILE_OPTIONS})
    foreach(var ${aux})
        set(configure_COMPILE_OPTIONS "${configure_COMPILE_OPTIONS}    list(APPEND INTERACTIVETOOLKIT_COMPILE_OPTIONS \"${var}\")\n")
    endforeach()

    #
    # LINK_LIBRARIES
    #
    to_unique_list(aux ${target_LINK_LIBRARIES})
    foreach(var ${aux})
        set(configure_LINK_LIBRARIES "${configure_LINK_LIBRARIES}    list(APPEND INTERACTIVETOOLKIT_LIBRARIES \"${var}\")\n")
    endforeach()

    #
    # LINK_OPTIONS
    #
    to_unique_list(aux ${target_LINK_OPTIONS})
    foreach(var ${aux})
        set(configure_LINK_OPTIONS "${configure_LINK_OPTIONS}    list(APPEND INTERACTIVETOOLKIT_LINK_OPTIONS \"${var}\")\n")
    endforeach()

    #
    # INCLUDE_DIRECTORIES
    #
    to_unique_list(aux ${target_INCLUDE_DIRECTORIES})
    foreach(var ${aux})
        set(configure_INCLUDE_DIRECTORIES "${configure_INCLUDE_DIRECTORIES}    list(APPEND INTERACTIVETOOLKIT_INCLUDE_DIRS \"${var}\")\n")
    endforeach()

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${inputfile}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${outputfile}"
        @ONLY
    )
endmacro()
