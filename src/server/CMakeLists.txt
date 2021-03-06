

include_directories(.)
#add_subdirectory(dir)
#add_sources(server
#	SourceFile.cpp
#)

# Suppress warnings
if(MSVC)
	# Conversions
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4305")
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	# Switches without all possible cases
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")
endif()
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(target_NAME hunter-server)
add_executable(${target_NAME} main.cpp ${server_SRC})
target_link_libraries(${target_NAME}
	${shared_LIB}
	${SFML_SYSTEM_LIBRARY}
	${SFML_NETWORK_LIBRARY}
)

# Copy data assets
add_custom_command(
	TARGET ${target_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/data"
	$<TARGET_FILE_DIR:${target_NAME}>
)
