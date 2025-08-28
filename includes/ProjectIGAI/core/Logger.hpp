#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <ProjectIGAI/core/Singleton.hpp>

#define DEFAULT_VERBOSITY_LEVEL 1
#define MAX_LOG_LEVEL 3
#define DEFAULT_COLOR_ESC "\033["

enum LogType {ERROR, INFO, PERFORMANCE, WARNING};

struct LogInfo {
    std::string toString;
    unsigned int verbosityTreshold;
    unsigned int color;
};

/**
 * @brief
 * 
 * Verbosity levels are :
 * 
 * - 0 : No logging
 * 
 * - 1 : Logging only Errors
 * 
 * - 2 : Logging precedent levels, Warning and Info
 * 
 * - 3 : Logging precedent levels and Performances
 * 
 */
class Logger : public Singleton<Logger>
{
    friend class Singleton<Logger>;

public:
    static void logError(const std::string& message) { Logger::getInstance().log(ERROR, message); }
    static void logInfo(const std::string& message) { Logger::getInstance().log(INFO, message); }
    static void logPerf(const std::string& message) { Logger::getInstance().log(PERFORMANCE, message); }
    static void logWarning(const std::string& message) { Logger::getInstance().log(WARNING, message); }

    static bool setVerbosity(const unsigned int& level);
    static void setColoredMode(const bool& mode) { getInstance().m_colored = mode; }

private:
    unsigned int m_verbosity = DEFAULT_VERBOSITY_LEVEL;
    bool m_colored = true;

    Logger() = default;

    void log(const LogType& type, const std::string& message);

    inline static std::unordered_map<LogType, LogInfo> s_logTypeValues = {
        {ERROR, {"ERROR", 1, 31}},

        {INFO, {"INFO", 2, 0}},
        {WARNING, {"WARNING", 2, 33}},

        {PERFORMANCE, {"PERFORMANCE", 3, 36}}
    };
};