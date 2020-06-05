add_executable(example_basic
    basic.cpp
 "BasicPlayer.h" "BasicPlayer.cpp" "utils.h")

target_link_libraries(example_basic
    engine
)