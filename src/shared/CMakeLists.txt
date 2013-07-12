

#add_subdirectory(dir)
add_sources(shared
	Sim.cpp
	Ship.cpp
	Cell.cpp
	PlayerDB.cpp
	common_sfml_network.cpp
)

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

# Shared static library so we don't have to redundantly compile shared code
add_library(shared ${shared_SRC})
require_libraries(shared
	shared
	${Boost_FILESYSTEM_LIBRARY}
)
if(UNIX)
	require_libraries(shared pthread)
endif()

propagate_groups()
