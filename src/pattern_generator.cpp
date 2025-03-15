#include "pattern_generator.hpp"
#include "globals.h"

#include <algorithm>
#include <numeric>
#include <random>
#include <thread>

namespace {
std::mt19937 random_engine{std::random_device{}()};
std::uniform_real_distribution<double> random_distribution{0.0, 1.0};
} // namespace

PatternGenerator::PatternGenerator(int w, int h) : m_w{w}, m_h{h} {
}

bool PatternGenerator::init() {
    m_texture = textureWrapper::createTexture(globals::renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, m_w, m_h);
    if (!m_texture) {
        return false;
    }

    m_pattern.resize(m_w * m_h);

    m_scales.push_back(Scale{200, 100, 0.05});
    m_scales.push_back(Scale{40, 20, 0.04});
    m_scales.push_back(Scale{20, 10, 0.03});
    m_scales.push_back(Scale{10, 5, 0.02});
    m_scales.push_back(Scale{2, 1, 0.01});

    randomize();

    return true;
}

void PatternGenerator::randomize() {
    std::generate(m_pattern.begin(), m_pattern.end(),
        [this]() { return random_distribution(random_engine); });
}

void PatternGenerator::step() {
    if (!m_scales.empty()) {
        int size = m_w * m_h;
        std::vector<double> activators(size);
        std::vector<double> inhibitors(size);
        std::vector<double> variations(size);
        std::vector<double> increments(size);

        for (const auto& scale : m_scales) {
            // Compute activator and inhibitor arrays
            std::thread inhibitorThread(&PatternGenerator::blur, this, scale.inhibitorRadius,
                std::ref(m_pattern), std::ref(inhibitors));
            std::thread activatorThread(&PatternGenerator::blur, this, scale.activatorRadius,
                std::ref(m_pattern), std::ref(activators));
            inhibitorThread.join();
            activatorThread.join();

            // Update the variation array if the variation for this element is smaller than the
            // one already stored. When processing the first scale, the variation array is
            // always updated, so we don't need to initialize it beforehand.
            for (int i{0}; i < size; i++) {
                double variation{activators[i] - inhibitors[i]};
                if ((&scale == &m_scales.front()) ||
                    (std::abs(variation) < std::abs(variations[i]))) {
                    variations[i] = variation;
                    increments[i] = (variation > 0) ? scale.increment : -scale.increment;
                }
            }
        }

        std::transform(m_pattern.begin(), m_pattern.end(), increments.begin(), m_pattern.begin(),
            std::plus<>{});

        normalize();
    }
}

bool PatternGenerator::render() {
    if (!m_texture) {
        return false;
    }

    void* pixels;
    int pitch;

    if (!SDL_LockTexture(m_texture.get(), nullptr, &pixels, &pitch)) {
        return false;
    }

    uint32_t* pixelColors = static_cast<uint32_t*>(pixels);
    for (int i{0}; i < m_pattern.size(); i++) {
        uint8_t color{static_cast<uint8_t>(m_pattern[i] * 0xFF)};
        pixelColors[i] = (color << 24) | (color << 16) | (color << 8) | SDL_ALPHA_OPAQUE;
    }

    SDL_UnlockTexture(m_texture.get());

    SDL_RenderTexture(globals::renderer, m_texture.get(), nullptr, nullptr);

    return true;
}

void PatternGenerator::normalize() {
    auto min = *std::min_element(m_pattern.begin(), m_pattern.end());
    auto max = *std::max_element(m_pattern.begin(), m_pattern.end());
    auto range = max - min;
    std::transform(m_pattern.begin(), m_pattern.end(), m_pattern.begin(),
        [min, range](auto element) { return (element - min) / range; });
}

void PatternGenerator::blur(int radius, const std::vector<double>& source,
    std::vector<double>& destination) const {
    std::vector<double> partialBlurring(m_w * m_h);
    blurHorizontal(radius, source, partialBlurring);
    blurVertical(radius, partialBlurring, destination);
}

void PatternGenerator::blurHorizontal(int radius, const std::vector<double>& source,
    std::vector<double>& destination) const {
    for (size_t y = 0; y < m_h; y++) {
        double sum = 0;
        int span = radius + 1;

        // In the blurred picture, the first pixel of each row is the average of the source pixels
        // between x = 0 and x = radius
        for (size_t x = 0; x <= radius; x++) {
            sum += source[x + y * m_w];
        }
        destination[y * m_w] = sum / static_cast<double>(span);

        // The other pixels are computed with a moving average. Pixel values are subtracted or added
        // from the sum only if they are part of the picture.
        for (int x = 1; x < m_w; x++) {
            if ((x + radius) < m_w) {
                sum += source[x + radius + y * m_w];
            } else {
                span--;
            }
            if ((static_cast<int>(x) - static_cast<int>(radius) - 1) >= 0) {
                sum -= source[x - radius - 1 + y * m_w];
            } else {
                span++;
            }

            destination[x + y * m_w] = sum / static_cast<double>(span);
        }
    }
}

void PatternGenerator::blurVertical(int radius, const std::vector<double>& source,
    std::vector<double>& destination) const {
    for (size_t x = 0; x < m_w; x++) {
        double sum = 0;
        int span = radius + 1;

        // In the blurred picture, the first pixel of each column is the average of the source
        // pixels between y = 0 and y = radius
        for (size_t y = 0; y <= radius; y++) {
            sum += source[x + y * m_w];
        }
        destination[x] = sum / static_cast<double>(span);

        // The other pixels are computed with a moving average. Pixel values are subtracted or added
        // from the sum only if they are part of the picture.
        for (int y = 1; y < m_h; y++) {
            if ((y + radius) < m_h) {
                sum += source[x + (y + radius) * m_w];
            } else {
                span--;
            }

            if ((static_cast<int>(y) - static_cast<int>(radius) - 1) >= 0) {
                sum -= source[x + (y - radius - 1) * m_w];
            } else {
                span++;
            }

            destination[x + y * m_w] = sum / static_cast<double>(span);
        }
    }
}
