#pragma once

#include <string>
#include <vector>
#include <optional>
#include "Renderer.h"

struct RendererConfig {
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
};

class CommandLineParser {
public:
    CommandLineParser(int argc, char* argv[]);
    
    // Parse command line arguments and return configuration
    RendererConfig parse();
    
    // Print usage information
    static void printUsage(const std::string& programName);
    
private:
    std::vector<std::string> args;
    
    // Helper methods for parsing specific arguments
    std::optional<std::string> parseStringArg(const std::string& argName);
    std::optional<int> parseIntArg(const std::string& argName);
    std::optional<float> parseFloatArg(const std::string& argName);
    std::optional<RenderMode> parseRenderModeArg(const std::string& argName);
    bool parseBoolArg(const std::string& argName);
}; 