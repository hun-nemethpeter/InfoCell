add_executable(edgetest
    EdgeTest.cc
)

target_include_directories(edgetest
    PUBLIC
        ${HEADER_ROOT_DIR}
)

target_link_libraries(edgetest
PUBLIC
    nlohmann_json::nlohmann_json
    fmt::fmt
    ftxui::ftxui
    gtest::gtest

    arc-lib
    config
    cells-lib
    cellstestbase
    util
)