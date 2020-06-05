add_executable(example_basic
    basic.cpp
	 pch.h "game_grid.h" "game_grid.cpp")

target_link_libraries(example_basic
    engine
)

target_precompile_headers(example_basic PRIVATE pch.h)