

cmake_minimum_required(VERSION 2.8)

project(hunter CXX C)


# Build in GNU profiling
# Other compiler id's: Clang, GNU, Intel, MSVC
if(PROFILE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		message("Building in CXX GNU profiling")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
	else()
		message("Requested CXX profiling but we don't have it for this compiler: " ${CMAKE_CXX_COMPILER_ID})
	endif()
endif()

# Subdirectories can create source file groups
# Usage: add_sources([group name] [source file name]...)
#        After, call propagate_groups() once in each directory
#        The variable [group name]_SRC will contain the source file paths
macro(add_sources)
	set(_arguments ${ARGN})
	list(GET _arguments 0 _group)
	list(REMOVE_AT _arguments 0)
	list(APPEND _groups ${_group})
	file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach(_src ${_arguments})
#		if(_relPath)
#			list(APPEND ${_group}_SRC "${_relPath}/${_src}")
#		else()
#			list(APPEND ${_group}_SRC "${_src}")
#		endif()
		list(APPEND ${_group}_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
	endforeach()
endmacro()

# Subdirectories can create external library groups
# Usage: require_libraries([group name] [library name]...)
#        After, call propagate_groups() once in each directory
#        The variable [group name]_LIB will contain the library names
macro(require_libraries)
	set(_arguments ${ARGN})
	list(GET _arguments 0 _group)
	list(REMOVE_AT _arguments 0)
	list(APPEND _groups ${_group})
	foreach(_lib ${_arguments})
		list(APPEND ${_group}_LIB "${_lib}")
	endforeach()
endmacro()

# Call this in each subdirectory AFTER all group related calls
macro(propagate_groups)
	file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	if(_relPath)
		set(_groups ${_groups} PARENT_SCOPE)
		foreach(_group ${_groups})
			set(${_group}_SRC ${${_group}_SRC} PARENT_SCOPE)
			set(${_group}_LIB ${${_group}_LIB} PARENT_SCOPE)
		endforeach()
	endif()
endmacro()



# Find OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
message(" OPENGL_INCLUDE_DIR: ${OPENGL_INCLUDE_DIR}")
message(" OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
message("OPENGL_gl_LIBRARY: ${OPENGL_gl_LIBRARY}")


# Find SFML
if (NOT "$ENV{SFML_ROOT}" STREQUAL "")
	set(CMAKE_MODULE_PATH $ENV{SFML_ROOT}/cmake/Modules ${CMAKE_MODULE_PATH})
endif ()
if (MSVC)
	set(SFML_STATIC_LIBRARIES TRUE)
endif ()
find_package(SFML REQUIRED COMPONENTS system window graphics network)
include_directories(${SFML_INCLUDE_DIR})
set(SFML_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${SFML_LIBRARIES})
message("SFML_LIBRARIES: ${SFML_LIBRARIES}")


# Find Boost ##################################################################
if(Boost_USE_STATIC_LIBS STREQUAL "")
	set(Boost_USE_STATIC_LIBS TRUE)
endif()
set(BOOST_ROOT_ENV $ENV{BOOST_ROOT})
if(BOOST_ROOT_ENV)
	# Get boost from a specifically defined folder
	set(BOOST_ROOT $ENV{BOOST_ROOT})
	message("BOOST_ROOT: " ${BOOST_ROOT})
elseif(WIN32)
	# Get boost from the ogre sdk
	set(BOOST_ROOT $ENV{OGRE_HOME}/boost)
	message("BOOST_ROOT: " ${BOOST_ROOT})
endif()
message("Boost_USE_STATIC_LIBS: ${Boost_USE_STATIC_LIBS}")
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIR})


# Work through the source tree
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_subdirectory(src)



