#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <Eigen/Dense>
#include <iostream>

// Custom formatter for Eigen matrices
template <typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
struct fmt::formatter<Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>>
{
    // Parses the format specifier (if any)
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    // Formats the Eigen matrix into the output
    template <typename FormatContext>
    auto format(const Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols> &mat, FormatContext &ctx) -> decltype(ctx.out())
    {
        auto out = ctx.out();
        for (int i = 0; i < mat.rows(); ++i)
        {
            for (int j = 0; j < mat.cols(); ++j)
            {
                out = fmt::format_to(out, "{}\t", mat(i, j));
            }
            out = fmt::format_to(out, "\n");
        }
        return out;
    }
};

int main()
{
    try
    {
        // Initialize logger
        auto logger = spdlog::basic_logger_mt("basic_logger", "logs/app.log");
        spdlog::set_default_logger(logger);
        spdlog::info("Program started successfully.");

        // Example Eigen matrix usage
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
