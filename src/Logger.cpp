#include <ProjectIGAI/Logger.hpp>

bool Logger::setVerbosity(const unsigned int &level)
{
    if(level < 0 || level > MAX_LOG_LEVEL) return false;
    
    getInstance().m_verbosity = level;
    logInfo("Verbosity level correctly set to " + std::to_string(getInstance().m_verbosity));
    return true;
}

void Logger::log(const LogType &type, const std::string &message)
{
    LogInfo infos = s_logTypeValues.at(type);
    if(getInstance().m_verbosity >= infos.verbosityTreshold) {
        std::cout <<
            DEFAULT_COLOR_ESC << infos.color << "m" <<
            "[" << infos.toString << "] " << message <<
            DEFAULT_COLOR_ESC << "0m" << std::endl;
    }
}