#pragma once

#include "texture_wrapper.h"

#include <SDL3/SDL_render.h>

#include <vector>

class PatternGenerator {
public:
    /// @param w Image width in pixels
    /// @param h Image height in pixels
    PatternGenerator(int width, int height);

    /// Initializes generator
    bool init();

    /// Randomizes image data
    void randomize();

    /// Generates next image step
    void step();

    /// Renders image
    bool render();

private:
    struct Scale {
        int activatorRadius;
        int inhibitorRadius;
        double increment;
    };

    int m_w;
    int m_h;
    std::vector<Scale> m_scales;
    std::vector<double> m_pattern;
    textureWrapper::unique_ptr_texture m_texture;

    /// Normalizes image data on the interval [0;1]
    void normalize();

    /// Box blur
    /// @param radius Blur radius
    /// @param source Data to blur
    /// @param destination Blurred data
    void blur(int radius, const std::vector<double>& source,
        std::vector<double>& destination) const;

    /// Box blur, horizontal pass
    /// @param radius Blur radius
    /// @param source Data to blur
    /// @param destination Blurred data
    void blurHorizontal(int radius, const std::vector<double>& source,
        std::vector<double>& destination) const;

    /// Box blur, vertical pass
    /// @param radius Blur radius
    /// @param source Data to blur
    /// @param destination Blurred data
    void blurVertical(int radius, const std::vector<double>& source,
        std::vector<double>& destination) const;
};
