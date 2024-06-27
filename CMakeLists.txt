cmake_minimum_required(VERSION 3.12)

# Name your project here
set(PROJECT_NAME "PortCE-Project")



project(${PROJECT_NAME})

set(SRC_DIR "./src")
set(SRC_PORTCE_DIR "./src_PortCE")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")



# Compiler
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Set C and C++ standards
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)

# Packages
find_package(SDL2 REQUIRED)

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp" "${SRC_PORTCE_DIR}/*.c")

# Create an executable
add_executable(${PROJECT_NAME} ${SRC_FILES} ${SRC_PORTCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_PORTCE_DIR}
	"${SRC_PORTCE_DIR}/PortCE_include/"
	"${SRC_PORTCE_DIR}/PortCE_include/ce/include/sys"
	"${SRC_PORTCE_DIR}/PortCE_include/keypadc"
	"${SRC_PORTCE_DIR}/PortCE_include/fileioc"
	"${SRC_PORTCE_DIR}/PortCE_include/ce/include"
	"${SRC_PORTCE_DIR}/PortCE_include/ce/include/ti"
	"${SRC_PORTCE_DIR}/PortCE_include/graphy"
)

# Compiler Flags Debug(-g -O0) Release(-O3)
set(OPT_FLAG -g -O0)
# -fsanitize= flags can be quite helpful in debugging segfaults.
set(SAN_FLAG)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${SAN_FLAG} -fcolor-diagnostics -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SAN_FLAG} -fcolor-diagnostics -fdiagnostics-color=always")

target_compile_options(
	${PROJECT_NAME} PUBLIC ${SAN_FLAG} -fcolor-diagnostics -fdiagnostics-color=always ${OPT_FLAG}
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

target_link_libraries(${PROJECT_NAME} PUBLIC SDL2 SDL2_mixer)