#**********************************************************************************
#  MIT License                                                                    *
#                                                                                 *
#  Copyright (c) 2023 Vahan Aghajanyan <vahancho@gmail.com>                       *
#                                                                                 *
#  Permission is hereby granted, free of charge, to any person obtaining a copy   *
#  of this software and associated documentation files (the "Software"), to deal  *
#  in the Software without restriction, including without limitation the rights   *
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
#  copies of the Software, and to permit persons to whom the Software is          *
#  furnished to do so, subject to the following conditions:                       *
#                                                                                 *
#  The above copyright notice and this permission notice shall be included in all *
#  copies or substantial portions of the Software.                                *
#                                                                                 *
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
#  SOFTWARE.                                                                      *
#*********************************************************************************/

cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

option(ENABLE_TESTING      "Enable unit test build" OFF)
option(ENABLE_EXAMPLES     "Enable examples build"  OFF)
option(BUILD_DOCUMENTATION "Build documentation"    OFF)

project(graphene
        VERSION 1.0.0
        DESCRIPTION "A header-only c++ library that provides implementation of an abstract graph."
        HOMEPAGE_URL "https://github.com/vahancho/graphene")

include(GNUInstallDirs)

add_library(${CMAKE_PROJECT_NAME} INTERFACE)

target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${CMAKE_PROJECT_NAME} INTERFACE cxx_std_17)

# Installation
install(TARGETS ${CMAKE_PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE gepaf::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES ${PROJECT_SOURCE_DIR}/src/graphene.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

###############################################################################
# Generate documentation if needed
#

# Check if Doxygen is installed
find_package(Doxygen)

if (BUILD_DOCUMENTATION)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_GENERATE_LATEX          NO)
        set(DOXYGEN_CREATE_SUBDIRS          YES)
        set(DOXYGEN_OUTPUT_DIRECTORY        ${CMAKE_BINARY_DIR}/docs/html)
        set(DOXYGEN_PROJECT_NAME            ${PROJECT_DESCRIPTION})
        set(DOXYGEN_EXTRACT_STATIC          YES)
        set(DOXYGEN_EXTRACT_ALL             YES)
        set(DOXYGEN_EXTRACT_LOCAL_METHODS   YES)
        set(DOXYGEN_WARN_NO_PARAMDOC        YES)
        set(DOXYGEN_RECURSIVE               YES)
        set(DOXYGEN_EXCLUDE                 ${PROJECT_SOURCE_DIR}/tes)
        set(DOXYGEN_HTML_OUTPUT             .)
        set(DOXYGEN_GENERATE_TREEVIEW       YES)
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE  README.md)

        # Add the documentation target. This target doesn't build by default if doxygen is found.
        # To build it explicitly, use '--target documentation' or 'make documentation' option.
        doxygen_add_docs(documentation
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         COMMENT "Building the project documentation")
    else()
        message("Failed to generate documentation: Doxygen is not found")
    endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable packaging with CPack
include(CPack)

if (ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()

if (ENABLE_TESTING)
    enable_testing()
    include(Dart)

    add_subdirectory(test)
endif()
