add_executable(example_basic
    basic.cpp
    Interface/CommandsUI.h
    Interface/CommandsUI.cpp
    CommandQueue.h
    CommandQueue.cpp
    pch.h
    )

target_link_libraries(example_basic
    engine
)

target_precompile_headers(example_basic PRIVATE pch.h)