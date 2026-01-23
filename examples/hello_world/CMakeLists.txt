cmake_minimum_required(VERSION 3.24)

# Name your project here
set(PROJECT_NAME "hello_world")

project(${PROJECT_NAME})

set(SRC_DIR "./src")
set(PORTCE_SOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

# Compiler
if(NOT EMSCRIPTEN)
	set(CMAKE_C_COMPILER clang)
	set(CMAKE_CXX_COMPILER clang++)
endif()

# Set C and C++ standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_COLOR_DIAGNOSTICS ON)

# Packages
find_package(PortCE CONFIG QUIET)
if(NOT PortCE_FOUND)
	add_subdirectory("${PORTCE_SOURCE_ROOT}" "${CMAKE_BINARY_DIR}/portce" EXCLUDE_FROM_ALL)
endif()
if(NOT PortCE_FOUND AND NOT TARGET PortCE::PortCE)
	message(FATAL_ERROR "PortCE not found. Install it or build from repo root so PortCE::PortCE is available.")
endif()

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")

# Create an executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/include
)

# Compiler Flags Debug(-g -O0) Release(-O2)
set(OPT_FLAG -g -O0)
# -fsanitize= flags can be quite helpful in debugging segfaults.
set(SAN_FLAG)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${SAN_FLAG}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SAN_FLAG}")

target_compile_options(
	${PROJECT_NAME} PUBLIC ${SAN_FLAG} ${OPT_FLAG}
	-Wall -Wextra -Wshadow

# Can help spot undefined behaviour when porting code from the eZ80
	-Wpointer-arith
	-Wpointer-bool-conversion
	-Wpointer-integer-compare
	-Wpointer-sign
	-Wpointer-to-enum-cast
	-Wpointer-to-int-cast
	-Wpointer-type-mismatch
#	-Wcast-align
	-Wcast-calling-convention
	-Wcast-function-type
	-Wcast-function-type-strict
	-Wcast-of-sel-type
#	-Wcast-qual
	-Wcast-qual-unrelated
	-Wchar-align
	-Wcompare-distinct-pointer-types
#	-Wconversion
	-Wconversion-null
)

target_link_libraries(${PROJECT_NAME} PUBLIC PortCE::PortCE)

include("${PORTCE_SOURCE_ROOT}/cmake/PortCEWeb.cmake")
portce_web_shell_config(
	${PROJECT_NAME}
	OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
	TEMPLATE "${PORTCE_SOURCE_ROOT}/web/portce_shell.html.in"
	TITLE ${PROJECT_NAME}
)
