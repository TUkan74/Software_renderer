#pragma once

#include <string>

/**
 * @brief Generates a test checkerboard texture
 * @param width Width of the texture
 * @param height Height of the texture
 * @param checkerSize Size of the checker pattern
 * @param filename Output filename
 * @return True if successful, false otherwise
 */
bool generateCheckerboardTexture(int width, int height, int checkerSize, const std::string& filename);

/**
 * @brief Generates a test gradient texture
 * @param width Width of the texture
 * @param height Height of the texture
 * @param filename Output filename
 * @return True if successful, false otherwise
 */
bool generateGradientTexture(int width, int height, const std::string& filename);

/**
 * @brief Generates various test textures and saves them to files
 */
void generateTestTextures(); 