#pragma once


#include <indicatorslib.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

namespace CaravelPM {
    /* Represents levels of logging.
     *
     */
   enum LogLevel {
      DEBUG,
      INFO,
      WARNING,
      ERROR,
      CRITICAL
   };



   class Logger {
   public:
       Logger(const std::string& filename);
       ~Logger();
       void write(LogLevel level, const std::string& msg);
       std::string levelToString(LogLevel level);
   private:
       /** Returns an output stream suitable for the current level
        *
        */
       std::ostream& logLevel(std::ostream& stream);
   private:
       std::ofstream m_LogFile;
       LogLevel m_CurrentLevel;
   };
};
