add_subdirectory(hybridcells)
add_subdirectory(tests)

add_library(arc-lib STATIC
    Color.cc
    Color.h
    Grid.cc
    Grid.h
    Observers.cc
    Observers.h
    Solver.cc
    Solver.h
    Task.cc
    Task.h
)

target_include_directories(arc-lib
    PUBLIC
        ${HEADER_ROOT_DIR}
)

target_link_libraries(arc-lib
	PUBLIC
		nlohmann_json::nlohmann_json
        fmt::fmt
	PRIVATE
        config
		utf8cpp
        cells-lib
        arc-hybridcells-lib
)