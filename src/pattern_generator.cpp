#include "pattern_generator.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <random>
#include <thread>

void PatternGenerator::init(int width, int height) {
    m_width = width;
    m_height = height;
    m_size = width * height;
    m_pattern.resize(m_size);

    m_scales.push_back(Scale{100, 50, 0.05});
    m_scales.push_back(Scale{50, 25, 0.04});
    m_scales.push_back(Scale{25, 12, 0.03});

    randomize();
}

const std::vector<double>& PatternGenerator::pattern() const {
    return m_pattern;
}

void PatternGenerator::randomize() {
    std::random_device device;
    std::default_random_engine engine(device());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    std::generate(m_pattern.begin(), m_pattern.end(),
        [&distribution, &engine]() { return distribution(engine); });
}

void PatternGenerator::normalize() {
    double min = *std::min_element(m_pattern.begin(), m_pattern.end());
    double max = *std::max_element(m_pattern.begin(), m_pattern.end());
    double range = max - min;
    std::transform(m_pattern.begin(), m_pattern.end(), m_pattern.begin(),
        [min, range](double element) { return (element - min) / range; });
}

void PatternGenerator::step() {
    if (!m_scales.empty()) {
        std::vector<double> activators(m_size);
        std::vector<double> inhibitors(m_size);
        std::vector<double> variations(m_size);
        std::vector<double> increments(m_size);

        for (Scale& scale : m_scales) {
            // Compute activator and inhibitor arrays
            std::thread inhibitorThread(&PatternGenerator::blur, this, scale.m_inhibitorRadius,
                std::ref(m_pattern), std::ref(inhibitors));
            std::thread activatorThread(&PatternGenerator::blur, this, scale.m_activatorRadius,
                std::ref(m_pattern), std::ref(activators));
            inhibitorThread.join();
            activatorThread.join();

            for (size_t i = 0; i < m_size; i++) {
                // Update the variation array if the variation for this element is smaller than the
                // one already stored. When processing the first scale, the variation array is
                // always updated, so we don't need to initialize it beforehand.
                double variation = activators[i] - inhibitors[i];
                if ((&scale == &m_scales.front()) ||
                    (std::abs(variation) < std::abs(variations[i]))) {
                    variations[i] = variation;
                    increments[i] = (variation > 0) ? scale.m_increment : -scale.m_increment;
                }
            }
        }

        std::transform(m_pattern.begin(), m_pattern.end(), increments.begin(), m_pattern.begin(),
            std::plus<>{});

        normalize();
    }
}

void PatternGenerator::blur(
    int radius, const std::vector<double>& source, std::vector<double>& destination) const {
    std::vector<double> partialBlurring(m_size);
    blurHorizontal(radius, source, partialBlurring);
    blurVertical(radius, partialBlurring, destination);
}

void PatternGenerator::blurHorizontal(
    int radius, const std::vector<double>& source, std::vector<double>& destination) const {
    for (size_t y = 0; y < m_height; y++) {
        double sum = 0;
        int span = radius + 1;

        // In the blurred picture, the first pixel of each row is the average of the source pixels
        // between x = 0 and x = radius
        for (size_t x = 0; x <= radius; x++) {
            sum += source[x + y * m_width];
        }
        destination[y * m_width] = sum / static_cast<double>(span);

        // The other pixels are computed with a moving average. Pixel values are subtracted or added
        // from the sum only if they are part of the picture.
        for (int x = 1; x < m_width; x++) {
            if ((x + radius) < m_width) {
                sum += source[x + radius + y * m_width];
            } else {
                span--;
            }

            if ((static_cast<int>(x) - static_cast<int>(radius) - 1) >= 0) {
                sum -= source[x - radius - 1 + y * m_width];
            } else {
                span++;
            }

            destination[x + y * m_width] = sum / static_cast<double>(span);
        }
    }
}

void PatternGenerator::blurVertical(
    int radius, const std::vector<double>& source, std::vector<double>& destination) const {
    for (size_t x = 0; x < m_width; x++) {
        double sum = 0;
        int span = radius + 1;

        // In the blurred picture, the first pixel of each column is the average of the source
        // pixels between y = 0 and y = radius
        for (size_t y = 0; y <= radius; y++) {
            sum += source[x + y * m_width];
        }
        destination[x] = sum / static_cast<double>(span);

        // The other pixels are computed with a moving average. Pixel values are subtracted or added
        // from the sum only if they are part of the picture.
        for (int y = 1; y < m_height; y++) {
            if ((y + radius) < m_height) {
                sum += source[x + (y + radius) * m_width];
            } else {
                span--;
            }

            if ((static_cast<int>(y) - static_cast<int>(radius) - 1) >= 0) {
                sum -= source[x + (y - radius - 1) * m_width];
            } else {
                span++;
            }

            destination[x + y * m_width] = sum / static_cast<double>(span);
        }
    }
}
