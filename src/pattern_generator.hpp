#pragma once

#include <vector>

class PatternGenerator {
public:
    /// @param width Image width in pixels
    /// @param height Image height in pixels
    PatternGenerator(int width, int height);

    /// Image data of size width*height pixels with values in [0;1]
    const std::vector<double>& pattern() const;

    /// Generate next pattern step
    void step();

    /// Randomize image data
    void randomize();

private:
    struct Scale {
        int m_activatorRadius;
        int m_inhibitorRadius;
        double m_increment;
    };

    int m_width;
    int m_height;
    int m_size;
    std::vector<Scale> m_scales;
    std::vector<double> m_pattern;

    /// Normalize image data on the interval [0;1]
    void normalize();

    /// Box blur
    /// @param radius Blur radius
    /// @param source Data to blur
    /// @param destination Blurred data
    void blur(
        int radius, const std::vector<double>& source, std::vector<double>& destination) const;

    /// Box blur, horizontal pass
    /// @param radius Blur radius
    /// @param source Data to blur
    /// @param destination Blurred data
    void blurHorizontal(
        int radius, const std::vector<double>& source, std::vector<double>& destination) const;

    /// Box blur, vertical pass
    /// @param radius Blur radius
    /// @param source Data to blur
    /// @param destination Blurred data
    void blurVertical(
        int radius, const std::vector<double>& source, std::vector<double>& destination) const;
};
