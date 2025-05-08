#include "CommandLineParser.h"
#include <iostream>
#include <stdexcept>

CommandLineParser::CommandLineParser(int argc, char* argv[]) {
    args.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
}

void CommandLineParser::printUsage(const std::string& programName) {
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

std::optional<std::string> CommandLineParser::parseStringArg(const std::string& argName) {
    for (size_t i = 0; i < args.size() - 1; ++i) {
        if (args[i] == argName) {
            return args[i + 1];
        }
    }
    return std::nullopt;
}

std::optional<int> CommandLineParser::parseIntArg(const std::string& argName) {
    auto value = parseStringArg(argName);
    if (value) {
        try {
            return std::stoi(*value);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid integer value for " + argName);
        }
    }
    return std::nullopt;
}

std::optional<float> CommandLineParser::parseFloatArg(const std::string& argName) {
    auto value = parseStringArg(argName);
    if (value) {
        try {
            return std::stof(*value);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid float value for " + argName);
        }
    }
    return std::nullopt;
}

std::optional<RenderMode> CommandLineParser::parseRenderModeArg(const std::string& argName) {
    auto value = parseStringArg(argName);
    if (value) {
        if (*value == "wireframe") return RenderMode::WIREFRAME;
        if (*value == "solid") return RenderMode::SOLID;
        if (*value == "textured") return RenderMode::TEXTURED;
        if (*value == "shaded") return RenderMode::TEXTURED_SHADED;
        if (*value == "colorful") return RenderMode::COLORFUL;
        throw std::runtime_error("Unknown rendering mode: " + *value);
    }
    return std::nullopt;
}

bool CommandLineParser::parseBoolArg(const std::string& argName) {
    for (const auto& arg : args) {
        if (arg == argName) {
            return true;
        }
    }
    return false;
}

RendererConfig CommandLineParser::parse() {
    RendererConfig config;
    
    // Check for help flag
    if (parseBoolArg("--help")) {
        printUsage(args[0]);
        exit(0);
    }
    
    // Parse all arguments
    if (auto input = parseStringArg("--input")) config.inputFile = *input;
    if (auto texture = parseStringArg("--texture")) config.textureFile = *texture;
    if (auto output = parseStringArg("--output")) config.outputFile = *output;
    if (auto width = parseIntArg("--width")) config.width = *width;
    if (auto height = parseIntArg("--height")) config.height = *height;
    if (auto mode = parseRenderModeArg("--mode")) config.renderMode = *mode;
    if (auto x = parseFloatArg("--camera-x")) config.cameraX = *x;
    if (auto y = parseFloatArg("--camera-y")) config.cameraY = *y;
    if (auto z = parseFloatArg("--camera-z")) config.cameraZ = *z;
    config.generateTestTextures = parseBoolArg("--generate-test-textures");
    
    // Validate required arguments
    if (config.inputFile.empty() && !config.generateTestTextures) {
        throw std::runtime_error("Input file is required unless --generate-test-textures is used");
    }
    
    return config;
} 