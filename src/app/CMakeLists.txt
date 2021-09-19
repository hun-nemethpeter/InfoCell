add_library(applib STATIC
App.cc
App.h
)
target_link_libraries(applib
	PUBLIC
		CONAN_PKG::nlohmann_json
		CONAN_PKG::utfcpp
)