# Currently, this CMake is only used by Android for React Native C++ autolinking
# We need to adjust this build if this is getting used by other platforms
# or to build Shared Object file, and may put this folder not this Android directory.

project("react-native-uws")
cmake_minimum_required(VERSION 3.14.0)

include(FetchContent)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ANDROID_STL_TYPE "c++_shared")
set(ANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES ON)

set(LIB_ANDROID_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(LIB_ANDROID_GENERATED_JNI_DIR ${LIB_ANDROID_DIR}/build/generated/source/codegen/jni)
set(LIB_CODEGEN_SPEC_NAME "ReactNativeUwsSpecs")

if(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a" OR CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,max-page-size=16384")
endif()

find_package(ReactAndroid REQUIRED CONFIG)
find_package(fbjni REQUIRED CONFIG)

file(
	GLOB_RECURSE
	LIB_SHARED_SRCS
	RELATIVE
	${CMAKE_CURRENT_SOURCE_DIR}
	"./*.cpp"
)

# It's useful later to call cpp to Java/Kotlin
# file(GLOB_RECURSE LIB_ANDROID_SRCS RELATIVE ${CMAKE_SOURCE_DIR} "src/main/cpp/*.cpp")
file(
	GLOB
	LIB_ANDROID_SRCS
	RELATIVE
	${LIB_ANDROID_DIR}
	"src/main/cpp/*.cpp"
)

# Codegen source files
file(
	GLOB_RECURSE
	LIB_CODEGEN_SRCS
	CONFIGURE_DEPENDS
	${LIB_ANDROID_GENERATED_JNI_DIR}/*.cpp
	${LIB_ANDROID_GENERATED_JNI_DIR}/**/*.cpp
)

add_library(
	${PROJECT_NAME}
	STATIC
	${LIB_SHARED_SRCS}
	${LIB_ANDROID_SRCS}
	${LIB_CODEGEN_SRCS}
)

# Add headers search paths
target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"."
	${LIB_ANDROID_SRCS}
	${LIB_ANDROID_GENERATED_JNI_DIR}
	"${LIB_ANDROID_GENERATED_JNI_DIR}/react/renderer/components/${LIB_CODEGEN_SPEC_NAME}"
)

# Add `app` headers
# target_include_directories(
# 	${PROJECT_NAME}
# 	INTERFACE
# 	"./app"
# )

# +++++ uWebSockets C++ library +++++
# Thanks to https://github.com/tony-go/cmake-uwebsockets/blob/main/CMakeLists.txt

FetchContent_Declare(
	rnuws_usockets_content
	GIT_REPOSITORY https://github.com/uNetworking/uSockets
	GIT_TAG v0.8.8
	GIT_SHALLOW ON
	GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(rnuws_usockets_content)

file(GLOB_RECURSE SOURCES ${rnuws_usockets_content_SOURCE_DIR}/src/*.c)
add_library(
	uSockets
	${SOURCES}
)
target_include_directories(uSockets PUBLIC "${rnuws_usockets_content_SOURCE_DIR}/src")
target_compile_definitions(uSockets PRIVATE LIBUS_NO_SSL)

FetchContent_Declare(
	rnuws_uwebsockets_content
	GIT_REPOSITORY https://github.com/uNetworking/uWebSockets
	GIT_TAG v20.78.0
	GIT_SHALLOW ON
	GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(rnuws_uwebsockets_content)
find_package(ZLIB REQUIRED)

# Create a folder to include the uWebSockets library with a scoped path
# like `#include "uWebSockets/App.h"

set(UWEBSOCKETS_WRAPPER_DIR ${CMAKE_BINARY_DIR}/uwebsockets_copy)

# 1.
# Remove existing directory "${CMAKE_BINARY_DIR}/uwebsockets_copy"
file(REMOVE_RECURSE ${UWEBSOCKETS_WRAPPER_DIR})

# 2.
# Make the directory again "${CMAKE_BINARY_DIR}/uwebsockets_copy"
file(
	MAKE_DIRECTORY
	${UWEBSOCKETS_WRAPPER_DIR}
)

# 3.
# Copy the "${rnuws_uwebsockets_content_SOURCE_DIR}/src" to "${UWEBSOCKETS_WRAPPER_DIR}"
# The result will be "${CMAKE_BINARY_DIR}/uwebsockets_copy/src/...cxxFiles..."
file(
	COPY
	"${rnuws_uwebsockets_content_SOURCE_DIR}/src"
	DESTINATION
	"${UWEBSOCKETS_WRAPPER_DIR}"
)

# 4.
# Rename "${UWEBSOCKETS_WRAPPER_DIR}/src" to ${UWEBSOCKETS_WRAPPER_DIR}/uWebSockets"
# Result from "${CMAKE_BINARY_DIR}/uwebsockets_copy/src" to "${CMAKE_BINARY_DIR}/uwebsockets_copy/uWebSockets"
file(
	RENAME
	"${UWEBSOCKETS_WRAPPER_DIR}/src"
	"${UWEBSOCKETS_WRAPPER_DIR}/uWebSockets"
)

add_library(uWebSockets INTERFACE)
target_include_directories(uWebSockets INTERFACE ${UWEBSOCKETS_WRAPPER_DIR})
target_link_libraries(uWebSockets INTERFACE uSockets ${ZLIB_LIBRARIES})
target_compile_options(uWebSockets INTERFACE -Wno-deprecated-declarations)

# ----- uWebSockets C++ library -----

add_subdirectory(app)

# Link Libraries
target_link_libraries(
	${PROJECT_NAME}
	ReactAndroid::reactnative
	ReactAndroid::jsi
	fbjni::fbjni
	react_codegen_ReactNativeUwsSpecs
	uWebSockets
	android
	log
	react_native_uws__app
)
