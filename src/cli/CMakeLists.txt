add_library(cli-lib STATIC
    App.cc
    App.h
)

target_link_libraries(cli-lib
PUBLIC
    CLI11::CLI11
    cells-lib
)

target_include_directories(cli-lib
PUBLIC
    ${HEADER_ROOT_DIR}
)

add_executable(arc-agi-solver
    main.cc
)

target_link_libraries(arc-agi-solver
PRIVATE
    cli-lib
)