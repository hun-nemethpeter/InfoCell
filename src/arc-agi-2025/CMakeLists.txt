add_library(arc-agi-2025-solver-lib STATIC
    App.cc
    App.h
    AppState.cc
    AppState.h
    Colors.cc
    Colors.h
    Solver.cc
    Solver.h
    Task.cc
    Task.h
)

target_link_libraries(arc-agi-2025-solver-lib
PUBLIC
    CLI11::CLI11
    cellslib
)

target_include_directories(arc-agi-2025-solver-lib
PUBLIC
    ${HEADER_ROOT_DIR}
)

add_executable(arc-agi-2025-solver
    main.cc
)

target_link_libraries(arc-agi-2025-solver
PRIVATE
    arc-agi-2025-solver-lib
)