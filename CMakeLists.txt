cmake_minimum_required(VERSION 3.29.3)

set(CMAKE_GENERATOR "Unix Makefiles")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wformat -Wno-unused-parameter -Wno-unused-function")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(kapi VERSION 0.0.0 LANGUAGES C)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif()

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler Flags: ${CMAKE_C_FLAGS}")

# CMake Tools
include(FindPkgConfig)
include(FetchContent)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(CPack)
include(CheckLibraryExists)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE PROJECT_HEADERS "src/*.h")
file(GLOB_RECURSE PROJECT_SOURCES "src/*.c")

set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
  json-c
  GIT_REPOSITORY https://github.com/json-c/json-c.git
  GIT_TAG        json-c-0.17-20230812
)
FetchContent_MakeAvailable(json-c)
set(BUILD_SHARED_LIBS ON)

add_executable(
  ${PROJECT_NAME}

  ${PROJECT_HEADERS}
  ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE json-c)

if (CMAKE_BUILD_TYPE STREQUAL "Release")

  target_compile_options(${PROJECT_NAME} PRIVATE -Os)

elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")

  target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG _DEBUG)

  target_compile_options(${PROJECT_NAME} PRIVATE
    -Og -pg -g -ggdb
    -Wall -Wextra -Wformat
    -fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope
  )

  target_link_options(${PROJECT_NAME} PRIVATE
    -fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope
  )

endif ()
