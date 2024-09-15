

#pragma once

#include <fstream>
#include <mutex>
#include <iostream>
#include <cstring>
#include <map>
#include <chrono>

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

constexpr char COMPILATION_OK[] = "\n\n{0}Compilation finished {1}successfully{2} ({3}){4}";
constexpr char COMPILATION_ERROR[] = "\n\n{0}Compilation exited {1}abnormally{2} ({3}){4}";

std::string indent(size_t length = TAB_LENGHT, char div = ' ');

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;


class Diagnostics {
public:

    Diagnostics(const char* log_file, LogLevel level) : dest(log_file), lvl(level) 
    {
        this->begin_time = Time::now();
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
    void error(Message message) 
    {
        log(message, LogLevel::ERROR);
        fsec duration = Time::now() - begin_time;
        std::cout << std::format(COMPILATION_ERROR, WHITE, RED, WHITE, std::chrono::duration_cast<ms>(duration), RESET) << std::endl;
        std::cout << '\n';
        exit(2); 
    }
    void critical(Message message) 
    {
        log(message, LogLevel::CRITICAL); 
        fsec duration = Time::now() - begin_time;
        std::cout << std::format(COMPILATION_ERROR, WHITE, RED, WHITE, std::chrono::duration_cast<ms>(duration), RESET) << std::endl;
        std::cout << '\n';
        exit(2); 
    }
private:

    void log(Message message, LogLevel level);

    std::ofstream dest;
    std::mutex mtx;
    LogLevel lvl;
    bool use_colors;
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_time;

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