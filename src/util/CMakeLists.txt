add_library(util STATIC
    ArcTask.cc
    ArcTask.h
    Logging.cc
    Logging.h
    LogMessage.h
)

target_link_libraries(util
	PUBLIC
		nlohmann_json::nlohmann_json
        applib
        cellslib
)