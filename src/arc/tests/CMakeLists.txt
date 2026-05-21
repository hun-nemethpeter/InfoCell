add_executable(arctest
    EdgeDetectorTest.cc
)

target_include_directories(arctest
    PUBLIC
        ${HEADER_ROOT_DIR}
)

target_link_libraries(arctest
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