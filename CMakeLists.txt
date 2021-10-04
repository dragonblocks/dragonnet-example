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

add_executable(DragonnetGenMessages
	${SOURCES_COMMON}
	"${DEPS_DIR}/dragonnet/gen_messages.c"
)

add_custom_command(
	OUTPUT "${CMAKE_SOURCE_DIR}/messages.h"
	COMMAND "${CMAKE_BINARY_DIR}/DragonnetGenMessages"
	MAIN_DEPENDENCY "${CMAKE_SOURCE_DIR}/messages.dnet"
	DEPENDS DragonnetGenMessages
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_target(ExampleMessages
	DEPENDS "${CMAKE_SOURCE_DIR}/messages.h"
)

add_executable(ExampleClient
	${SOURCES_COMMON}
	client.c
)

add_dependencies(ExampleClient ExampleMessages)

add_executable(ExampleServer
	${SOURCES_COMMON}
	server.c
)

add_dependencies(ExampleServer ExampleMessages)
