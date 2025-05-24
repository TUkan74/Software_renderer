#pragma once

#include <spdlog/spdlog.h>
#include <iostream>
#include <memory>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void info(const std::string& message) {
        spdlog::info(message);
        std::cout << "[INFO] " << message << std::endl;
    }

    void warn(const std::string& message) {
        spdlog::warn(message);
        std::cout << "[WARN] " << message << std::endl;
    }

    void error(const std::string& message) {
        spdlog::error(message);
        std::cout << "[ERROR] " << message << std::endl;
    }

    template<typename... Args>
    void info(const std::string& format, Args&&... args) {
        spdlog::info(format, std::forward<Args>(args)...);
        std::cout << "[INFO] " << fmt::format(format, std::forward<Args>(args)...) << std::endl;
    }

    template<typename... Args>
    void warn(const std::string& format, Args&&... args) {
        spdlog::warn(format, std::forward<Args>(args)...);
        std::cout << "[WARN] " << fmt::format(format, std::forward<Args>(args)...) << std::endl;
    }

    template<typename... Args>
    void error(const std::string& format, Args&&... args) {
        spdlog::error(format, std::forward<Args>(args)...);
        std::cout << "[ERROR] " << fmt::format(format, std::forward<Args>(args)...) << std::endl;
    }

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
}; 