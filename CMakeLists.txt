# Cmake version
cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

# Project name
project(V3CImmersivePlatform VERSION 1.0 LANGUAGES CXX)

# CPP standard
if(ANDROID)
	set(CMAKE_CXX_STANDARD 17)
else()
	set(CMAKE_CXX_STANDARD 20)
endif()

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# rpath options
if(UNIX)
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	SET(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH} ${CMAKE_INSTALL_PREFIX}/lib)
endif()

# Compiler options
if(WIN32)
	add_compile_definitions(NOMINMAX) # disable min/max macros
endif()

if(MSVC)

	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4244 /wd4267 /wd4305) # disable warning about pod conversion
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4996) # disable warning about unsafe parameters function
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4125) # decimal digit terminates octal escape sequence
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4706) # assignment within conditional expression
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd26451) # arithmetic overflow
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4127) #	conditional expression is constant
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd6262) # consider moving some data to heap
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4068) # unkown pragma
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4005) # disable warning about macro redefinition
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4100) # disable warning about unreferenced formal parameter
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4245) # disable warning about conversion
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4018) # disable warning about signed/unsigned mismatch
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4389) # disable warning about signed/unsigned mismatch
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4189) # disable warning about variable initialized but not referenced
	LIST(APPEND MSVC_COMPILE_OPTIONS /wd4102) # disable warning about unreferenced lable
	LIST(APPEND MSVC_COMPILE_OPTIONS /W4) # Show warnings until level 4
	#LIST(APPEND MSVC_COMPILE_OPTIONS /WX) # Process warnings as errors

	add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:${MSVC_COMPILE_OPTIONS}>")

elseif(UNIX)

	LIST(APPEND UNIX_COMPILE_OPTIONS -Wall -Wextra -Wshadow) # Show almost all warnings
	LIST(APPEND UNIX_COMPILE_OPTIONS -Wfatal-errors) # Stop compile at first error

	add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:AppleClang,Clang,GNU>>:${UNIX_COMPILE_OPTIONS}>")

endif()

# Useful macros
macro(subdir_list result curdir)
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	set(dirlist "")
	foreach(child ${children})
		if(IS_DIRECTORY ${curdir}/${child} AND NOT ${child} STREQUAL ".svn")
			list(APPEND dirlist ${child})
		endif()
	endforeach()
	set(${result} ${dirlist})
endmacro()

# Source files
file(GLOB_RECURSE ALL_HEADER_FILES *.h)
file(GLOB_RECURSE ALL_TEMPLATE_FILES *.hpp)
file(GLOB_RECURSE ALL_C_FILES *.c)
file(GLOB_RECURSE ALL_CPP_FILES *.cpp)

# Beautifier
find_program(CLANG_FORMAT NAMES "clang-format" DOC "Path to clang-format executable")
if(CLANG_FORMAT)
	add_custom_target(
		beautifier
		COMMAND ${CLANG_FORMAT}
		-style=file
		-i
		${ALL_CPP_FILES} ${ALL_C_FILES} ${ALL_HEADER_FILES} ${ALL_TEMPLATE_FILES}
	)
endif()

# Linter
option(ENABLE_CLANG_TIDY "Turn on clang linter" OFF)
if(ENABLE_CLANG_TIDY)
	add_definitions("-DCLANG_TIDY_ENABLED")
	find_program(CLANG_TIDY NAMES "clang-tidy" DOC "Path to clang-tidy executable")
	if(CLANG_TIDY)
		option(CLANG_TIDY_FIX "Enable linter auto fix" OFF)
		if(CLANG_TIDY_FIX)
			set(CLANG_TIDY_OPTION "-fix")
		endif()
		set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_OPTION})
	endif()
endif()

# Uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

# Thirdparty
find_package(iloj REQUIRED)
find_package(TMIV REQUIRED)
if(ANDROID)
	find_package(FMT REQUIRED)
	find_package(ilojj REQUIRED)
endif()

# Logging configuration
option(ENABLE_LOGGING "Enable logging" ON)
if(ENABLE_LOGGING)
	add_definitions("-DENABLE_LOGGER")
endif()

# DASH configuration
if(USE_DASH)
	message (STATUS "Test if use dash streaming -- In Use")
	add_compile_definitions(DASH_STREAMING)
endif()

# uvgRTP configuration
if(USE_UVG_RTP)
	message (STATUS "Test if use uvg rtp streaming -- In Use")
	add_compile_definitions(UVG_RTP_STREAMING)
endif()

# WebRTC configuration
if(USE_WEBRTC_RTP)
	message (STATUS "Test if use webrtc rtp streaming -- In Use")
	add_compile_definitions(WEBRTC_RTP_STREAMING)
endif()

# streaming Log configuration
set (_USE_STREAMING_LOG ${USE_STREAMING_LOG})
if(_USE_STREAMING_LOG MATCHES "true")
	ADD_DEFINITIONS(-DMEASUREMENT_LOG)
endif()


# Haptic configuration
option(USE_HAPTIC "Use haptic" OFF)
message (STATUS "Test if use haptic")
if(USE_HAPTIC)
	message (STATUS "Test if use haptic -- In Use")
	add_compile_definitions(HAPTIC)
	#add_definitions("-DHAPTIC")
else()
	message (STATUS "Test if use haptic -- Not Used")
endif()

# Components
add_subdirectory("Sources")
