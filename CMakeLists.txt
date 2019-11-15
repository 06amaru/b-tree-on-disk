cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(utecdf C CXX)

# - conda environment -----------------------------------------------------------------------------
if ("$ENV{CONDA_BUILD}" STREQUAL "1")
    set(PREFIX $ENV{PREFIX})
    set(CMAKE_SYSTEM_PREFIX_PATH "$ENV{BUILD_PREFIX};$ENV{PREFIX};${CMAKE_SYSTEM_PREFIX_PATH}")
    message(STATUS "Conda build detected, CMAKE_SYSTEM_PREFIX_PATH set to: ${CMAKE_SYSTEM_PREFIX_PATH}")
elseif (DEFINED ENV{CONDA_PREFIX})
    set(CMAKE_SYSTEM_PREFIX_PATH "$ENV{CONDA_PREFIX};${CMAKE_SYSTEM_PREFIX_PATH}")
    set(PREFIX $ENV{CONDA_PREFIX})
    message(STATUS "Conda environment detected, CMAKE_SYSTEM_PREFIX_PATH set to: ${CMAKE_SYSTEM_PREFIX_PATH}")
endif ()
set(CMAKE_CXX_STANDARD 14)

include(FeatureSummary)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CTest)

find_package(Threads)
find_package(GTest REQUIRED)
find_package(fmt REQUIRED)
enable_testing ()

include_directories(
    $ENV{PROJECT_SOURCE_DIR}/include
    $ENV{CONDA_PREFIX}/include 
    $ENV{PREFIX}/include
)

link_directories($ENV{CONDA_PREFIX}/lib)

if (GTEST_FOUND)
    enable_testing ()
    add_custom_target (coverage
            COMMAND lcov -c -d ${CMAKE_BINARY_DIR}/src -o coverage.info
            COMMAND lcov -r coverage.info '/usr*' '*boost*' '*build*' -o coverage.info
            COMMAND genhtml coverage.info -o ${CMAKE_BINARY_DIR}/coverage-html)
endif ()

function (blazingdb_artifact)
  set (OPTIONS OPTIONAL VIRTUAL)
  set (ONE_VALUE_ARGS PREFIX TARGET)
  set (MULTI_VALUE_ARGS SOURCES INCLUDES LIBRARIES PUBLIC_HEADERS TESTS)
  cmake_parse_arguments (ARG "${OPTIONS}" "${ONE_VALUE_ARGS}"
    "${MULTI_VALUE_ARGS}" ${ARGN})
  if (ARG_UNPARSED_ARGUMENTS)
    message (SEND_ERROR
      "Error: unrecognized arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif ()

  if (${ARG_VIRTUAL})
    set (TYPE OBJECT)
  endif ()

  if (ARG_SOURCES)
    add_library (${ARG_TARGET} ${TYPE} ${ARG_SOURCES})
  endif()

  if (ARG_LIBRARIES)
    target_link_libraries (${ARG_TARGET} ${ARG_LIBRARIES})
  endif ()

  if (ARG_INCLUDES)
    target_include_directories (${ARG_TARGET} ${ARG_INCLUDES})
  endif ()

  if (ARG_LIBRARY_PATHS)
    target_link_libraries (${ARG_TARGET} ${ARG_LIBRARY_PATHS})
  endif ()

  if (NOT ARG_PREFIX)
    set (ARG_PREFIX "")
  endif ()

  install (TARGETS ${ARG_TARGET}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

  if (ARG_PUBLIC_HEADERS)
    message (STATUS "Log: public headers ${ARG_PUBLIC_HEADERS}")
    install(DIRECTORY ${ARG_PUBLIC_HEADERS} DESTINATION include)
  endif ()

  if (GTEST_FOUND AND ARG_TESTS)
    message (STATUS "Log: unit test ${ARG_TARGET}-gtest")
    add_executable (${ARG_TARGET}-gtest ${ARG_TESTS})
    target_include_directories (${ARG_TARGET} PUBLIC src)
    target_compile_options (${ARG_TARGET} PUBLIC -ggdb -O0)
    target_link_libraries (${ARG_TARGET}-gtest 
        ${ARG_TARGET}
        GTest::GTest
        $ENV{CONDA_PREFIX}/lib/libgmock_main.a
        $ENV{CONDA_PREFIX}/lib/libgmock.a )
        
    add_custom_target (${ARG_TARGET}-gtest-run COMMAND $<TARGET_FILE:${ARG_TARGET}-gtest>)

    add_test(NAME ${ARG_TARGET}-gtest COMMAND ${CMAKE_BINARY_DIR}/${ARG_TARGET}-gtest)

    add_dependencies (coverage ${ARG_TARGET}-gtest-run)
  endif ()
endfunction ()


blazingdb_artifact (
        TARGET
            btree

        INCLUDES PUBLIC 
            $ENV{CONDA_PREFIX}/include

        PUBLIC_HEADERS
          ${CMAKE_CURRENT_SOURCE_DIR}/src/utec

        LIBRARIES
            fmt::fmt

        PREFIX
            utec

        SOURCES
            src/utec/disk/pagemanager.cpp

        TESTS
            tests/utec/memory/btree_test.cpp
            tests/utec/disk/btree_test.cpp

)