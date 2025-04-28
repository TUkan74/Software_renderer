#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <Eigen/Dense>
#include <iostream>
#include <filesystem>
#include "Application.h"
#include "TestTexture.h"

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

void printUsage(const std::string& programName) {
    std::cout << "Software Renderer - A simple 3D rendering engine" << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                   Display this help message" << std::endl;
    std::cout << "  --input <obj_file>       Input OBJ model file (required unless --generate-test-textures is used)" << std::endl;
    std::cout << "  --texture <tga_file>     Input TGA texture file" << std::endl;
    std::cout << "  --output <tga_file>      Output TGA image file (default: output.tga)" << std::endl;
    std::cout << "  --width <pixels>         Width of the output image (default: 800)" << std::endl;
    std::cout << "  --height <pixels>        Height of the output image (default: 600)" << std::endl;
    std::cout << "  --mode <mode>            Rendering mode (default: wireframe)" << std::endl;
    std::cout << "                           Modes: wireframe, solid, textured, shaded" << std::endl;
    std::cout << "  --camera-x <value>       Camera X position (default: 0)" << std::endl;
    std::cout << "  --camera-y <value>       Camera Y position (default: 0)" << std::endl;
    std::cout << "  --camera-z <value>       Camera Z position (default: 5)" << std::endl;
    std::cout << "  --generate-test-textures Generate test textures in the examples directory" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Create logs directory if it doesn't exist
        if (!fs::exists("logs")) {
            fs::create_directory("logs");
        }
        
        // Initialize logger
        auto logger = spdlog::basic_logger_mt("basic_logger", "logs/app.log");
        spdlog::set_default_logger(logger);
        spdlog::info("Software Renderer started");
        
        // Default parameter values
        std::string inputFile;
        std::string textureFile;
        std::string outputFile = "output.tga";
        int width = 800;
        int height = 600;
        RenderMode renderMode = RenderMode::WIREFRAME;
        float cameraX = 0.0f;
        float cameraY = 0.0f;
        float cameraZ = 5.0f;
        bool generateTestTextures = false;
        
        // Parse command-line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                printUsage(argv[0]);
                return 0;
            }
            else if (arg == "--input") {
                if (i + 1 < argc) {
                    inputFile = argv[++i];
                }
            }
            else if (arg == "--texture") {
                if (i + 1 < argc) {
                    textureFile = argv[++i];
                }
            }
            else if (arg == "--output") {
                if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            }
            else if (arg == "--width") {
                if (i + 1 < argc) {
                    width = std::stoi(argv[++i]);
                }
            }
            else if (arg == "--height") {
                if (i + 1 < argc) {
                    height = std::stoi(argv[++i]);
                }
            }
            else if (arg == "--mode") {
                if (i + 1 < argc) {
                    std::string mode = argv[++i];
                    if (mode == "wireframe") {
                        renderMode = RenderMode::WIREFRAME;
                    }
                    else if (mode == "solid") {
                        renderMode = RenderMode::SOLID;
                    }
                    else if (mode == "textured") {
                        renderMode = RenderMode::TEXTURED;
                    }
                    else if (mode == "shaded") {
                        renderMode = RenderMode::TEXTURED_SHADED;
                    }
                    else if (mode == "colorful") {
                        renderMode = RenderMode::COLORFUL;
                    }
                    else {
                        std::cerr << "Unknown rendering mode: " << mode << std::endl;
                        printUsage(argv[0]);
                        return 1;
                    }
                }
            }
            else if (arg == "--camera-x") {
                if (i + 1 < argc) {
                    cameraX = std::stof(argv[++i]);
                }
            }
            else if (arg == "--camera-y") {
                if (i + 1 < argc) {
                    cameraY = std::stof(argv[++i]);
                }
            }
            else if (arg == "--camera-z") {
                if (i + 1 < argc) {
                    cameraZ = std::stof(argv[++i]);
                }
            }
            else if (arg == "--generate-test-textures") {
                generateTestTextures = true;
            }
        }
        
        // Create examples directory if needed
        if (!fs::exists("examples")) {
            fs::create_directory("examples");
        }
        
        // Generate test textures if requested
        if (generateTestTextures) {
            ::generateTestTextures();
            std::cout << "Test textures generated in the examples directory." << std::endl;
            
            // If no input file is specified, exit after generating textures
            if (inputFile.empty()) {
                return 0;
            }
        }
        
        // Validate input
        if (inputFile.empty()) {
            std::cerr << "Error: Input file is required" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        
        spdlog::info("Configuration:");
        spdlog::info("  Input file: {}", inputFile);
        spdlog::info("  Texture file: {}", textureFile.empty() ? "none" : textureFile);
        spdlog::info("  Output file: {}", outputFile);
        spdlog::info("  Dimensions: {}x{}", width, height);
        spdlog::info("  Camera position: ({}, {}, {})", cameraX, cameraY, cameraZ);
        
        // Initialize the application
        Application app(width, height);
        if (!app.initialize()) {
            spdlog::error("Failed to initialize application");
            return 1;
        }
        
        // Load the model
        if (!app.loadModel(inputFile)) {
            spdlog::error("Failed to load model from {}", inputFile);
            return 1;
        }
        
        // Load texture if specified
        if (!textureFile.empty()) {
            if (!app.setTexture(textureFile)) {
                spdlog::error("Failed to load texture from {}", textureFile);
                return 1;
            }
        }
        
        // Set camera position
        app.setCameraPosition(cameraX, cameraY, cameraZ);
        
        // Set rendering mode
        app.setRenderMode(renderMode);
        
        // Render the model
        if (!app.render()) {
            spdlog::error("Failed to render model");
            return 1;
        }
        
        // Save the output image
        if (!app.saveImage(outputFile)) {
            spdlog::error("Failed to save image to {}", outputFile);
            return 1;
        }
        
        spdlog::info("Rendering completed. Output saved to {}", outputFile);
        std::cout << "Rendering completed. Output saved to " << outputFile << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        spdlog::error("Unhandled exception: {}", e.what());
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
