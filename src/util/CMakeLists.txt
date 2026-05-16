add_library(util INTERFACE
    Log.h
)
target_include_directories(config INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}
    fmt::fmt
    spdlog::spdlog
)