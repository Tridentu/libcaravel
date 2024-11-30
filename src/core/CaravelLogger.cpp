
#include "CaravelLogger.h"
#include <indicatorslib.hpp>


std::ostream& CaravelPM::Logger::logLevel(std::ostream& stream) {
    switch(m_CurrentLevel){
        case CaravelPM::LogLevel::DEBUG:
            stream << termcolor::bold;
            break;
        case CaravelPM::LogLevel::INFO:
            break;
        case CaravelPM::LogLevel::WARNING:
            stream << termcolor::bold << termcolor::yellow;
            break;
        case CaravelPM::LogLevel::ERROR:
            stream << termcolor::red;
            break;
        case CaravelPM::LogLevel::CRITICAL:
            stream <<  termcolor::bold << termcolor::red;
            break;
    }
    return stream;
}


CaravelPM::Logger::Logger(const std::string& filename){
    m_LogFile.open(filename, std::ios::app);
    if (!m_LogFile.is_open()) {
        std::cerr << termcolor::red << "[Caravel Error] Error opening log file." << termcolor::reset << std::endl;
    }
}

CaravelPM::Logger::~Logger(){
    m_LogFile.close();
}

void CaravelPM::Logger::write(LogLevel level, const std::string& msg){
    if(m_CurrentLevel != level){
        m_CurrentLevel = level;
    }
    switch(m_CurrentLevel){
        case INFO:
        case DEBUG:
              logLevel(std::cout) << "[Caravel]: "  << msg << termcolor::reset << std::endl;
              break;
        case WARNING:
              logLevel(std::cout) << "[Caravel][Warn]: "  << msg << termcolor::reset << std::endl;
            break;
        case ERROR:
        case CRITICAL:
              logLevel(std::cout) << "[Caravel][Error]: " << msg << termcolor::reset << std::endl;
            break;
    }
    time_t now = time(0);
    tm* timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S", timeinfo);
    std::ostringstream logEntry;
    logEntry << "[" << timestamp << "] "
    << levelToString(m_CurrentLevel) << ": " << msg
    << std::endl;
    if (m_LogFile.is_open()) {
        m_LogFile << logEntry.str();
        m_LogFile
        .flush(); // Ensure immediate write to file
    }
}

std::string CaravelPM::Logger::levelToString(CaravelPM::LogLevel level){
    switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}
