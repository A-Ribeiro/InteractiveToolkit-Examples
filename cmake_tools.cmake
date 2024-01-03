# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

macro ( tool_make_global _var )
    set ( ${_var} ${${_var}} CACHE INTERNAL "hide this!" FORCE )
endmacro( )

macro ( tool_append_if_not_exists_and_make_global _var _value )
    foreach(entry IN ITEMS ${${_var}})
        if("${entry}" STREQUAL "${_value}")
            return()
        endif()
    endforeach()
    set(${_var} ${${_var}} "${_value}")
    tool_make_global(${_var})
endmacro( )

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


macro(copy_file_after_build PROJECT_NAME)
    foreach(FILENAME IN ITEMS ${ARGN})
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
			get_filename_component(FILENAME_WITHOUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}" NAME)
            add_custom_command(
                TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${FILENAME_WITHOUT_PATH} )
        elseif(EXISTS "${FILENAME}")
			get_filename_component(FILENAME_WITHOUT_PATH "${FILENAME}" NAME)
            add_custom_command(
                TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${FILENAME}
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${FILENAME_WITHOUT_PATH} )
        else()
            message(FATAL_ERROR "File Does Not Exists: ${FILENAME}")
        endif()
    endforeach()
endmacro()

macro(copy_directory_after_build PROJECT_NAME)
    foreach(DIRECTORY IN ITEMS ${ARGN})
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}")
            add_custom_command(
                TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DIRECTORY} )
        else()
            message(FATAL_ERROR "Directory Does Not Exists: ${DIRECTORY}")
        endif()
    endforeach()
endmacro()