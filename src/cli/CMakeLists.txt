add_library(cli-lib STATIC
    App.cc
    App.h
)

target_link_libraries(cli-lib
PUBLIC
    CLI11::CLI11
    arc-lib
    cells-lib
    config
)

target_include_directories(cli-lib
PUBLIC
    ${HEADER_ROOT_DIR}
)

add_executable(infocell-cli
    main.cc
)

target_link_libraries(infocell-cli
PRIVATE
    cli-lib
)