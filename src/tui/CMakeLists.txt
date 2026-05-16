add_library(tuilib STATIC
    App.cc
    App.h
    Logging.cc
    Logging.h
    LogMessage.h
    Scroller.cc
    Scroller.h
    Tester.cc
    Tester.h
)

if(MSVC)
    target_compile_options(tuilib PUBLIC "/Zc:__cplusplus")
endif()

target_include_directories(tuilib
    PUBLIC
        ${HEADER_ROOT_DIR}
)

target_link_libraries(tuilib
	PUBLIC
		nlohmann_json::nlohmann_json
        ftxui::ftxui
        fmt::fmt
	PRIVATE
        arc-lib
        cells-lib
        config
		utf8cpp
)