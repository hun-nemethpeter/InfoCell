add_library(util STATIC
    ArcTask.cc
    ArcTask.h
    Logging.cc
    Logging.h
    LogMessage.h
)


target_link_libraries(util
	PUBLIC
		CONAN_PKG::nlohmann_json
        cellslib
)