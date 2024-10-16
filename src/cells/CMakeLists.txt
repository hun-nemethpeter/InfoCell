add_library(cellslib STATIC
    Arc.cc
    Arc.h
    Cells.cc
    Cells.h
    Brain.cc
    Brain.h
)
if(MSVC)
    target_compile_options(cellslib PUBLIC "/Zc:__cplusplus")
endif()
target_include_directories(cellslib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(cellslib
    PUBLIC
        utf8cpp
        boost::boost
        Freetype::Freetype
        gtest::gtest
        fmt::fmt
        spdlog::spdlog
)

add_library(celltestbase STATIC
    StructPrinter.cc
    StructPrinter.h
    SVGPrinter.cc
    SVGPrinter.h
    SVGStructPrinter.cc
    SVGStructPrinter.h
    ValuePrinter.cc
    ValuePrinter.h
    CellTestBase.cc
    CellTestBase.h
)

target_link_libraries(celltestbase
PUBLIC
    config
    cellslib
    applib
    fsvguilib
)

add_executable(celltest
    CellTest.cc
)
target_link_libraries(celltest
PRIVATE
    celltestbase
)

add_executable(edgetest
    EdgeTest.cc
)
target_link_libraries(edgetest
PRIVATE
    celltestbase
)