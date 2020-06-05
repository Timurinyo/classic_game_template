add_executable(example_basic
    basic.cpp
 "Interface/CommandsUI.h" "Interface/CommandsUI.cpp" "CommandQueue.h" "CommandQueue.cpp")

target_link_libraries(example_basic
    engine
)