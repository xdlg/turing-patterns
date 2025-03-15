#pragma once

#include <SDL3/SDL_render.h>

#include <memory>

namespace textureWrapper {

struct TextureDeleter {
    void operator()(SDL_Texture* texture) const {
        SDL_DestroyTexture(texture);
    }
};

typedef std::unique_ptr<SDL_Texture, TextureDeleter> unique_ptr_texture;

inline unique_ptr_texture createTexture(SDL_Renderer* renderer, SDL_PixelFormat format,
    SDL_TextureAccess access, int w, int h) {
    return unique_ptr_texture(SDL_CreateTexture(renderer, format, access, w, h), TextureDeleter());
}

} // namespace textureWrapper
