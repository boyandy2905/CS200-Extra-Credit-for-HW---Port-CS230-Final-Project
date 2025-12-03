/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace CS230
{
    namespace {
        const char* to_cstr(Logger::Severity s) noexcept
        {
            switch (s)
            {
            case Logger::Severity::Verbose: return "VERBOSE";
            case Logger::Severity::Debug:   return "DEBUG";
            case Logger::Severity::Event:   return "EVENT";
            case Logger::Severity::Error:   return "ERROR";
            }
            return "UNKNOWN";
        }
    }

    Logger::Logger(Severity severity, bool use_console, std::chrono::system_clock::time_point start_time_in)
        : min_level(severity),
          out_stream(nullptr),
          start_time(start_time_in)
    {
        if (use_console)
        {
            out_stream.rdbuf(std::cout.rdbuf());
        }
        else
        {
            file_stream.open("log.txt", std::ios::out | std::ios::trunc);
            if (file_stream.is_open())
            {
                out_stream.rdbuf(file_stream.rdbuf());
            }
            else
            {
                out_stream.rdbuf(std::cerr.rdbuf());
                out_stream << "[Logger] Failed to open log.txt; falling back to std::cerr\n";
            }
        }
    }

    void Logger::LogError(std::string text)   { log(Severity::Error, std::move(text)); }
    void Logger::LogEvent(std::string text)   { log(Severity::Event, std::move(text)); }
    void Logger::LogDebug(std::string text)   { log(Severity::Debug, std::move(text)); }
    void Logger::LogVerbose(std::string text) { log(Severity::Verbose, std::move(text)); }

    void Logger::log(Severity severity, std::string message)
    {
        if (static_cast<int>(severity) < static_cast<int>(min_level))
            return;

        std::ostringstream line;
        line << std::fixed << std::setprecision(3)
             << "[" << std::setw(8) << seconds_since_start() << "s] "
             << "[" << to_cstr(severity) << "] "
             << message << '\n';

        out_stream << line.str();
        out_stream.flush();
    }
}
