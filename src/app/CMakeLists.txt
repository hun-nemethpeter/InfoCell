add_library(applib STATIC
    App.cc
    App.h
    Colors.h
    Picture.cc
    Picture.h
    Scroller.cc
    Scroller.h
    Solver.cc
    Solver.h
)

if(MSVC)
    target_compile_options(applib PUBLIC "/Zc:__cplusplus")
endif()

target_include_directories(applib
    PUBLIC
        ${HEADER_ROOT_DIR}
)

target_link_libraries(applib
	PUBLIC
		nlohmann_json::nlohmann_json
        ftxui::ftxui
        fmt::fmt
	PRIVATE
        config
		utf8cpp
        cellslib
        tests
        util
)