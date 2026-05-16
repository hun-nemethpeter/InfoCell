add_library(arc-hybridcells-lib STATIC
    DeprecatedCells.cc
    DeprecatedCells.h
    Frame.cc
    Frame.h
    Grid.cc
    Grid.h
    Pixel.cc
    Pixel.h
    Shape.cc
    Shape.h
)

target_include_directories(arc-hybridcells-lib
    PUBLIC
        ${HEADER_ROOT_DIR}
)

target_link_libraries(arc-hybridcells-lib
	PUBLIC
        fmt::fmt
	PRIVATE
        config
        cells-lib
        util
)