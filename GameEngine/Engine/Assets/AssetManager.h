#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>
#include <unordered_map>
#include <iostream>

struct TextureAsset
{
    SDL_Texture* texture = nullptr;
    std::string path = "";
};

class AssetManager
{
public:
    AssetManager() = default;
    ~AssetManager();

    void Init(SDL_Renderer* renderer);

    SDL_Texture* LoadTexture(
        const std::string& key,
        const std::string& path
    );

    SDL_Texture* GetTexture(const std::string& key) const;

    std::string GetTexturePath(const std::string& key) const;

    bool HasTexture(const std::string& key) const;

    bool RemoveTexture(const std::string& key);

    void ClearTextures();

    const std::unordered_map<std::string, TextureAsset>& GetTextures() const
    {
        return m_textures;
    }

private:
    SDL_Renderer* m_renderer = nullptr;
    std::unordered_map<std::string, TextureAsset> m_textures;
};