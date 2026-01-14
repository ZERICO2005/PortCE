cmake_minimum_required(VERSION 3.24)

# Name your project here
set(PROJECT_NAME "hello_world")

project(${PROJECT_NAME})

set(SRC_DIR "./src")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

# Compiler
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Set C and C++ standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_COLOR_DIAGNOSTICS ON)

# Packages
find_package(PortCE REQUIRED)

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
