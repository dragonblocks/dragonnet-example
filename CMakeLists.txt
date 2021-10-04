cmake_minimum_required(VERSION 3.12)

project(DragonnetExample)

set(CMAKE_BUILD_TYPE Debug)

link_libraries(
	pthread
)

set(DEPS_DIR "${CMAKE_SOURCE_DIR}/deps/")

include_directories(SYSTEM
	${DEPS_DIR}
)

add_compile_options(-Wall -Wextra -Werror -fmax-errors=4)

set(SOURCES_COMMON
	"${DEPS_DIR}/dragontype/array.c"
	"${DEPS_DIR}/dragontype/bintree.c"
	"${DEPS_DIR}/dragontype/list.c"
	"${DEPS_DIR}/dragontype/number.c"
	"${DEPS_DIR}/dragontype/queue.c"
	"${DEPS_DIR}/dragonport/asprintf.c"
	"${DEPS_DIR}/dragonnet/addr.c"
	"${DEPS_DIR}/dragonnet/listen.c"
	"${DEPS_DIR}/dragonnet/peer.c"
	"${DEPS_DIR}/dragonnet/recv_thread.c"
)

add_executable(DragonnetTypegen
	${SOURCES_COMMON}
	"${DEPS_DIR}/dragonnet/typegen/main.c"
)

add_custom_command(
	OUTPUT "${CMAKE_SOURCE_DIR}/dnet-types.h"
	COMMAND "${CMAKE_BINARY_DIR}/DragonnetTypegen"
	MAIN_DEPENDENCY "${CMAKE_SOURCE_DIR}/types.dnet"
	DEPENDS DragonnetTypegen
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_target(ExampleTypes
	DEPENDS "${CMAKE_SOURCE_DIR}/dnet-types.h"
)

add_executable(ExampleCS
	${SOURCES_COMMON}
	cs_example.c
)

add_dependencies(ExampleCS ExampleTypes)
