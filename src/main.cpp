#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <Eigen/Dense>
#include <iostream>
#include <filesystem>
#include "Application.h"
#include "TestTexture.h"
#include "CommandLineParser.h"

namespace fs = std::filesystem;

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

int main(int argc, char* argv[]) {
    try {
        // Create logs directory if it doesn't exist
        if (!fs::exists("logs")) {
            fs::create_directory("logs");
        }
        
        // Initialize logger with both console and file sinks
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/app.log", true);
        
        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
        auto logger = std::make_shared<spdlog::logger>("basic_logger", sinks.begin(), sinks.end());
        spdlog::set_default_logger(logger);
        
        spdlog::info("Software Renderer started");
        
        // Parse command line arguments
        CommandLineParser parser(argc, argv);
        RendererConfig config = parser.parse();
        
        // Create examples directory if needed
        if (!fs::exists("examples")) {
            fs::create_directory("examples");
        }
        
        // Generate test textures if requested
        if (config.generateTestTextures) {
            ::generateTestTextures();
            
            // If no input file is specified, exit after generating textures
            if (config.inputFile.empty()) {
                return 0;
            }
        }
        
        spdlog::info("Configuration:");
        spdlog::info("  Input file: {}", config.inputFile);
        spdlog::info("  Texture file: {}", config.textureFile.empty() ? "none" : config.textureFile);
        spdlog::info("  Output file: {}", config.outputFile);
        spdlog::info("  Dimensions: {}x{}", config.width, config.height);
        spdlog::info("  Camera position: ({}, {}, {})", config.cameraX, config.cameraY, config.cameraZ);
        
        // Initialize the application
        Application app(config.width, config.height);
        if (!app.initialize()) {
            spdlog::error("Failed to initialize application");
            return 1;
        }
        
        // Load the model
        if (!app.loadModel(config.inputFile)) {
            spdlog::error("Failed to load model from {}", config.inputFile);
            return 1;
        }
        
        // Load texture if specified
        if (!config.textureFile.empty()) {
            if (!app.setTexture(config.textureFile)) {
                spdlog::error("Failed to load texture from {}", config.textureFile);
                return 1;
            }
        }
        
        // Set camera position
        app.setCameraPosition(config.cameraX, config.cameraY, config.cameraZ);
        
        // Set rendering mode
        app.setRenderMode(config.renderMode);
        
        // Render the model
        if (!app.render()) {
            spdlog::error("Failed to render model");
            return 1;
        }
        
        // Save the output image
        if (!app.saveImage(config.outputFile)) {
            spdlog::error("Failed to save image to {}", config.outputFile);
            return 1;
        }
        
        spdlog::info("Rendering completed. Output saved to {}", config.outputFile);
        
        return 0;
    }
    catch (const std::exception& e) {
        spdlog::error("Unhandled exception: {}", e.what());
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
