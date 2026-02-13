#pragma once
#include <string_view>
#include <print>
#include <chrono>
#include <format>
#include <source_location>


enum class LogLevel {
    Trace, Debug, Info, Warning, Error
};

struct LogFormat {
    std::string_view fmt;
    std::source_location loc;

    template<typename T>
    consteval LogFormat(const T& s, const std::source_location& l = std::source_location::current())
        : fmt(s), loc(l) {}
};

class Logger {
public:
    Logger() = delete;

    template<typename... Args>
    static void log(LogLevel level, 
                    LogFormat format, 
                    Args&&... args) {
        
        std::string_view levelStr;
        std::string_view color;

        switch (level) {
            case LogLevel::Trace:   levelStr = "[TRACE]";   color = "\033[90m"; break;
            case LogLevel::Debug:   levelStr = "[DEBUG]";   color = "\033[36m"; break;
            case LogLevel::Info:    levelStr = "[INFO] ";   color = "\033[32m"; break;
            case LogLevel::Warning: levelStr = "[WARN] ";   color = "\033[33m"; break;
            case LogLevel::Error:   levelStr = "[ERROR]";   color = "\033[31m"; break;
        }

        auto now = std::chrono::system_clock::now();
        auto nowSeconds = std::chrono::floor<std::chrono::seconds>(now);
        
        // Estraiamo il nome del file ridotto
        std::string_view fullPath = format.loc.file_name();
        auto fileName = fullPath.substr(fullPath.find_last_of("/\\") + 1);

        // Estraiamo il nome della funzione ridotto
        std::string_view funcName = format.loc.function_name();
        auto firstColonPos = funcName.find_first_of("::");
        auto lastColonPos = funcName.find_first_of("(");
        if (lastColonPos != std::string_view::npos) {
            funcName = funcName.substr(0, lastColonPos);
        }
        if (firstColonPos != std::string_view::npos) {
            funcName.remove_prefix(firstColonPos + 2);
        }
        
        // Stampa: [Ora] [LIVELLO] [File:Riga @ Funzione] Messaggio
        std::print("{}{:%T} {} [{}:{} @ {}()] ", 
                   color, nowSeconds, levelStr, fileName, format.loc.line(), funcName);
        
        // Use vprint_unicode to print using the runtime format string
        // Note: This bypasses compile-time format string checking but fixes the ambiguity 
        // between variadic args and default source_location argument.
        std::vprint_unicode(format.fmt, std::make_format_args(args...));

        std::print("\033[0m\n");
    }

    // Scorciatoie
    template<typename... Args> 
    static void trace(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Trace, fmt, std::forward<Args>(args)...); }

    template<typename... Args> 
    static void debug(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Debug, fmt, std::forward<Args>(args)...); }

    template<typename... Args> 
    static void info(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Info, fmt, std::forward<Args>(args)...); }

    template<typename... Args> 
    static void warn(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Warning, fmt, std::forward<Args>(args)...); }

    template<typename... Args> 
    static void error(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Error, fmt, std::forward<Args>(args)...); }
};