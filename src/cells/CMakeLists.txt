add_subdirectory(printers)
add_subdirectory(tests)

add_library(cells-lib STATIC
    Cells.cc
    Cells.h
    Brain.cc
    Brain.h
    Compiler.cc
    Compiler.h
    NodeBase.cc
    NodeBase.h
    ToolFinder.cc
    ToolFinder.h
)
target_include_directories(cells-lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(cells-lib
    PUBLIC
        fmt::fmt
        spdlog::spdlog
        utf8cpp
        util
)