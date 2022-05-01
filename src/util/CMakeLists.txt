add_library(util STATIC
    Converter.cc
    Converter.h
    Logging.cc
    Logging.h
)


target_link_libraries(util
	PUBLIC
		CONAN_PKG::nlohmann_json
        cellslib
)