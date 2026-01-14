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
    
    /**
     * @brief Log a message as an error in user console (with adapted color and verbosity level
     * applied).
     * 
     * @param message message to log
     */
    static void logError(const std::string& message) { Logger::getInstance().log(ERROR, message); }

    /**
     * @brief Log a message as an information in user console (with adapted color and verbosity
     * level applied).
     * 
     * @param message message to log
     */
    static void logInfo(const std::string& message) { Logger::getInstance().log(INFO, message); }

    /**
     * @brief Log a message as a performance info in user console (with adapted color and verbosity
     * level applied).
     * 
     * @param message message to log
     */
    static void logPerf(const std::string& message) { Logger::getInstance().log(PERFORMANCE, message); }

    /**
     * @brief Log a message as a warning in user console (with adapted color and verbosity level
     * applied).
     * 
     * @param message message to log
     */
    static void logWarning(const std::string& message) { Logger::getInstance().log(WARNING, message); }

    /**
     * @brief Set the Logger global verbosity
     * 
     * @param level new level of verbosity
     * @return true if correctly modified, false otherwise
     */
    static bool setVerbosity(const unsigned int& level);

    /**
     * @brief Allow to switch between B&W verbosity and colored verbosity.
     * 
     * @param mode new mode for Logger
     */
    static void setColoredMode(const bool& mode) { getInstance().m_colored = mode; }

private:

    /** Verbosity level */
    unsigned int m_verbosity = DEFAULT_VERBOSITY_LEVEL;
    
    /** Coloration mode */
    bool m_colored = true;

    // Constructor is hidded (singleton pattern)
    Logger() = default;

    /**
     * @brief Base function to log a message.
     * 
     * @param type logging type
     * @param message message to log
     */
    void log(const LogType& type, const std::string& message);

    /** Map of logging types wih associated colors */
    inline static std::unordered_map<LogType, LogInfo> s_logTypeValues = {
        {ERROR, {"ERROR", 1, 31}},

        {INFO, {"INFO", 2, 0}},
        {WARNING, {"WARNING", 2, 33}},

        {PERFORMANCE, {"PERFORMANCE", 3, 36}}
    };
};