add_executable(example_basic
    basic.cpp
    BasicPlayer.h
    BasicPlayer.cpp
    utils.h
    Interface/CommandsUI.h
    Interface/CommandsUI.cpp
    CommandQueue.h
    CommandQueue.cpp
    pch.h
    game_grid.cpp game_grid.h
    tileset.cpp tileset.h
    game_tile.cpp game_tile.h
	PoorsManStateMachine.h PoorsManStateMachine.cpp
    game_level.cpp game_level.h)

target_link_libraries(example_basic
    engine
)

target_precompile_headers(example_basic PRIVATE pch.h)