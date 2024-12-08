#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <Eigen/Dense>
#include <iostream>

int main()
{
    try
    {
        // Initialize spdlog logger
        auto logger = spdlog::basic_logger_mt("basic_logger", "logs/app.log");
        spdlog::set_default_logger(logger);
        spdlog::info("Program started successfully.");

        // Example usage of Eigen
        Eigen::Matrix2d mat;
        mat(0, 0) = 3;
        mat(1, 0) = 2.5;
        mat(0, 1) = -1;
        mat(1, 1) = mat(1, 0) + mat(0, 1);
        spdlog::info("Eigen matrix created:\n{}", mat);

        std::cout << "Matrix:\n"
                  << mat << std::endl;
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
