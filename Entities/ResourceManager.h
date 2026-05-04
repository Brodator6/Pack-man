#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
    using TexturePtr = std::shared_ptr<SDL_Texture>;

    ResourceManager() = default;
    explicit ResourceManager(SDL_Renderer *renderer) : renderer(renderer) {}

    std::shared_ptr<SDL_Texture> getTexture(const std::string &path) {
        if (path.empty() || renderer == nullptr) {
            return nullptr;
        }

        auto cacheIt = textureCache.find(path);
        if (cacheIt != textureCache.end()) {
            if (auto cachedTexture = cacheIt->second.lock()) {
                return cachedTexture;
            }
            textureCache.erase(cacheIt);
        }

        SDL_Texture *rawTexture = IMG_LoadTexture(renderer, path.c_str());
        if (rawTexture == nullptr) {
            return nullptr;
        }

        TexturePtr texture(rawTexture, SDL_DestroyTexture);
        textureCache[path] = texture;
        return texture;
    }

    void setRenderer(SDL_Renderer *renderer) {
        this->renderer = renderer;
    }

private:
    SDL_Renderer *renderer = nullptr;
    std::unordered_map<std::string, std::weak_ptr<SDL_Texture>> textureCache;
};
