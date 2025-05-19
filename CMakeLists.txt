cmake_minimum_required(VERSION 3.28)

project(asteroids)

include(FetchContent)

FetchContent_Declare(
	raylib
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
	GIT_TAG        5.5
)

set(FETCHCONTENT_QUIET OFF)

FetchContent_MakeAvailable(raylib)

add_executable(${PROJECT_NAME}
	src/explosions.c
	src/game.c
	src/gameState.c
	src/highScore.c
	src/main.c
	src/playerShip.c
	src/postGame.c
	src/records.c
	src/resources.c
	src/rocks.c
	src/saucer.c
	src/title.c
)

# Shortcut to build executable into project's root directory
set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY                ${PROJECT_SOURCE_DIR}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${PROJECT_SOURCE_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${PROJECT_SOURCE_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${PROJECT_SOURCE_DIR}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
