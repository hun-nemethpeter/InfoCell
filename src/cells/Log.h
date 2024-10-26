#define IS_LOG_ENABLED (SPDLOG_ACTIVE_LEVEL != SPDLOG_LEVEL_OFF)

#pragma execution_character_set("utf-8")
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#define TRACE(category, msg, ...) SPDLOG_LOGGER_TRACE(spdlog::get(#category), msg, __VA_ARGS__)
#define DEBUG(category, msg, ...) SPDLOG_LOGGER_DEBUG(spdlog::get(#category), msg, __VA_ARGS__)
#define INFO(category, msg, ...) SPDLOG_LOGGER_INFO(spdlog::get(#category), msg, __VA_ARGS__)
#define WARN(category, msg, ...) SPDLOG_LOGGER_WARN(spdlog::get(#category), msg, __VA_ARGS__)
#define ERROR(category, msg, ...) SPDLOG_LOGGER_ERROR(spdlog::get(#category), msg, __VA_ARGS__)