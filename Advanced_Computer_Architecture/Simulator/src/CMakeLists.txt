set(CMAKE_CXX STANDARD 11)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(simulator simulator.cpp ALU.cpp execute.cpp decode.cpp fetch.cpp)
