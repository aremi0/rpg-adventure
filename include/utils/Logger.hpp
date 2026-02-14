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
        
        std::string_view level_str;
        std::string_view color;

        switch (level) {
            case LogLevel::Trace:   level_str = "[TRACE]";   color = "\033[90m"; break;
            case LogLevel::Debug:   level_str = "[DEBUG]";   color = "\033[36m"; break;
            case LogLevel::Info:    level_str = "[INFO] ";   color = "\033[32m"; break;
            case LogLevel::Warning: level_str = "[WARN] ";   color = "\033[33m"; break;
            case LogLevel::Error:   level_str = "[ERROR]";   color = "\033[31m"; break;
        }

        auto now = std::chrono::system_clock::now();
        auto now_seconds = std::chrono::floor<std::chrono::seconds>(now);
        
        // Estraiamo il nome del file ridotto
        std::string_view full_path = format.loc.file_name();
        auto file_name = full_path.substr(full_path.find_last_of("/\\") + 1);

        // Estraiamo il nome della funzione ridotto
        std::string_view func_name = format.loc.function_name();
        auto first_colon_pos = func_name.find_first_of("::");
        auto last_colon_pos = func_name.find_first_of("(");
        if (last_colon_pos != std::string_view::npos) {
            func_name = func_name.substr(0, last_colon_pos);
        }
        if (first_colon_pos != std::string_view::npos) {
            func_name.remove_prefix(first_colon_pos + 2);
        }
        
        // Stampa: [Ora] [LIVELLO] [File:Riga @ Funzione] Messaggio
        std::print("{}{:%T} {} [{}:{} @ {}()] ", 
                   color, now_seconds, level_str, file_name, format.loc.line(), func_name);
        
        // Use vprint_unicode to print using the runtime format string
        // Note: This bypasses compile-time format string checking but fixes the ambiguity 
        // between variadic args and default source_location argument.
        std::vprint_unicode(format.fmt, std::make_format_args(args...));

        std::print("\033[0m\n");
    }

    // Scorciatoie
    template<typename... Args> 
    static void Trace(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Trace, fmt, std::forward<Args>(args)...); }

    template<typename... Args> 
    static void Debug(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Debug, fmt, std::forward<Args>(args)...); }

    template<typename... Args> 
    static void Info(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Info, fmt, std::forward<Args>(args)...); }

    template<typename... Args> 
    static void Warn(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Warning, fmt, std::forward<Args>(args)...); }

    template<typename... Args> 
    static void Error(LogFormat fmt, Args&&... args) 
    { log(LogLevel::Error, fmt, std::forward<Args>(args)...); }
};