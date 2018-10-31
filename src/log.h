#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/sources/channel_logger.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

// Register the global logger. this can be accessed easily with app_logger::get
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(app_logger, src::logger_mt)

void init(const std::string& log_name)
{
    // Add common attributes (timestamp, lineID...)
    logging::add_common_attributes();

    logging::add_file_log
    (    keywords::file_name = log_name + "_%N.log",
         keywords::rotation_size = 10 * 1024 * 1024,
         keywords::auto_flush = true,
         keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
         keywords::format = "[%TimeStamp%][%Channel%]: %Message%"
    );
}