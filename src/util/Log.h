#define IS_LOG_ENABLED (SPDLOG_ACTIVE_LEVEL != SPDLOG_LEVEL_OFF)

#pragma execution_character_set("utf-8")
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#define INFOCELLS_THROW_EXCEPTION_ON_ERROR 1

#define TRACE(category, ...) SPDLOG_LOGGER_TRACE(spdlog::get(#category), __VA_ARGS__)
#define DEBUG(category, ...) SPDLOG_LOGGER_DEBUG(spdlog::get(#category), __VA_ARGS__)
#define INFO(category, ...) SPDLOG_LOGGER_INFO(spdlog::get(#category), __VA_ARGS__)
#define WARN(category, ...) SPDLOG_LOGGER_WARN(spdlog::get(#category), __VA_ARGS__)

#if INFOCELLS_THROW_EXCEPTION_ON_ERROR
#define INFOCELLS_ERROR_STATEMENT(category, ...) (SPDLOG_LOGGER_ERROR(spdlog::get(#category), __VA_ARGS__), throw fmt::format(__VA_ARGS__))
#else
#define INFOCELLS_ERROR_STATEMENT(category, ...) SPDLOG_LOGGER_ERROR(spdlog::get(#category), __VA_ARGS__)
#endif

#define ERROR(category, ...) INFOCELLS_ERROR_STATEMENT(category, __VA_ARGS__)
//#define ERROR(category, msg, ...) SPDLOG_LOGGER_ERROR(spdlog::get(#category), msg, __VA_ARGS__)
