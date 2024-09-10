

#pragma once

#include <fstream>
#include <mutex>
#include <iostream>
#include <cstring>
#include <map>

#include "token.hh"

enum class LogLevel {
    INFO        = 0,
    WARNING     = 1,
    ERROR       = 2,
    CRITICAL    = 3
};

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define PURPLE  "\033[1;35m"
#define BLUE    "\033[1:34m"
#define WHITE   "\033[1;37m"
#define GREEN   "\033[1;32m"

#define TAB_LENGHT 4

std::string indent(size_t length = TAB_LENGHT);


class Diagnostics {
public:

    Diagnostics(const char* log_file, LogLevel level) : dest(log_file), lvl(level) 
    {
        if (strcmp(log_file, "") != 0)
        {
            dest.open(log_file, std::ios_base::app);
            if (!dest.is_open())
            {
                std::cerr << "Unable to open output file \'" << log_file << "\', please check if the file exists or has read permission set!" << std::endl;
                exit(2);
            }
        }
        use_colors = true;
    }

    ~Diagnostics()
    {
        if (dest.is_open())
        {
            dest.close();
        }
    }

    void info(Message message) { log(message, LogLevel::INFO); }
    void warning(Message message) { log(message, LogLevel::WARNING); }
    void error(Message message) { log(message, LogLevel::ERROR); }
    void critical(Message message) { log(message, LogLevel::CRITICAL); }
private:

    void log(Message message, LogLevel level);

    std::ofstream dest;
    std::mutex mtx;
    LogLevel lvl;
    bool use_colors;

    std::map<LogLevel, std::string> color_codes = 
    {
        { LogLevel::INFO,       BLUE },
        { LogLevel::WARNING,    PURPLE },
        { LogLevel::ERROR,      RED },
        { LogLevel::CRITICAL,   RED }
    };

    std::map<LogLevel, std::string> level_names = 
    {
        { LogLevel::INFO,       "Info:" },
        { LogLevel::WARNING,    "Warning:" },
        { LogLevel::ERROR,      "Error:" },
        { LogLevel::CRITICAL,   "Critical:" }
    };
};