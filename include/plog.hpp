#pragma once

// all in one
#include "async_file_appender.hpp"
#include "file_writer_type.hpp"
#include "log.hpp"
#include "log_appender_interface.hpp"
#include "log_file.hpp"
#include "log_buffer.hpp"
#include "log_config.hpp"
#include "log_level.hpp"

#define log_fatal(fmt, args...) plog::Logger::Fatal(fmt, ##args)
#define log_error(fmt, args...) plog::Logger::Error(fmt, ##args)
#define log_warn(fmt, args...) plog::Logger::Warn(fmt, ##args)
#define log_debug(fmt, args...) plog::Logger::Debug(fmt, ##args)
#define log_info(fmt, args...) plog::Logger::Info(fmt, ##args)
