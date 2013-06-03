

include_directories(.)
#add_subdirectory(dir)

# Suppress switch warnings
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")
endif()
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(target_NAME hunter-server)
add_executable(${target_NAME} main.cpp ${server_SRC})
target_link_libraries(${target_NAME}
	${shared_LIB}
)

# Copy data assets
add_custom_command(
	TARGET ${target_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/data"
	$<TARGET_FILE_DIR:${target_NAME}>
)
