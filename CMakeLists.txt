cmake_minimum_required(VERSION 3.16)
project(ERL_HUB_MAIT_OOPS LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(reliefhub src/ReliefHub.cpp)
target_include_directories(reliefhub PUBLIC include)

add_executable(erl_hub src/main.cpp)
target_link_libraries(erl_hub PRIVATE reliefhub)

add_executable(erl_hub_tests tests/test.cpp)
target_link_libraries(erl_hub_tests PRIVATE reliefhub)

enable_testing()
add_test(NAME reliefhub_tests COMMAND erl_hub_tests)
