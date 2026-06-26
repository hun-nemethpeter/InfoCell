add_library(cellstestbase STATIC
    CellTestBase.cc
    CellTestBase.h
    TestLib.cc
    TestLib.h
)

target_link_libraries(cellstestbase
PUBLIC
    cells-printers
    cells-lib
    gtest::gtest
    util
    fmt::fmt
    spdlog::spdlog
)

target_include_directories(cellstestbase
    PUBLIC
        ${HEADER_ROOT_DIR}
)

add_executable(cellstest
    CellTest.cc
)

target_link_libraries(cellstest
PRIVATE
    nlohmann_json::nlohmann_json
    ftxui::ftxui

    arc-hybridcells-lib
    cellstestbase
    config
    tuilib
)