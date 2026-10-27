#include "AssetManager.h"

AssetManager::~AssetManager()
{
    ClearTextures();
}

void AssetManager::Init(SDL_Renderer* renderer)
{
    m_renderer = renderer;
}

SDL_Texture* AssetManager::LoadTexture(
    const std::string& key,
    const std::string& path
)
{
    if (!m_renderer)
    {
        std::cout << "AssetManager renderer is null!" << std::endl;
        return nullptr;
    }

    auto it = m_textures.find(key);

    if (it != m_textures.end())
    {
        return it->second.texture;
    }

    SDL_Texture* texture = IMG_LoadTexture(m_renderer, path.c_str());

    if (!texture)
    {
        std::cout << "Texture load failed: " << path << std::endl;
        std::cout << IMG_GetError() << std::endl;
        return nullptr;
    }

    TextureAsset asset;
    asset.texture = texture;
    asset.path = path;

    m_textures[key] = asset;

    std::cout << "Texture loaded: " << key << " -> " << path << std::endl;

    return texture;
}

SDL_Texture* AssetManager::GetTexture(const std::string& key) const
{
    auto it = m_textures.find(key);

    if (it == m_textures.end())
        return nullptr;

    return it->second.texture;
}

std::string AssetManager::GetTexturePath(const std::string& key) const
{
    auto it = m_textures.find(key);

    if (it == m_textures.end())
        return "";

    return it->second.path;
}

bool AssetManager::HasTexture(const std::string& key) const
{
    return m_textures.find(key) != m_textures.end();
}

bool AssetManager::RemoveTexture(const std::string& key)
{
    auto it = m_textures.find(key);

    if (it == m_textures.end())
        return false;

    if (it->second.texture)
        SDL_DestroyTexture(it->second.texture);

    m_textures.erase(it);
    return true;
}

void AssetManager::ClearTextures()
{
    for (auto& pair : m_textures)
    {
        if (pair.second.texture)
            SDL_DestroyTexture(pair.second.texture);
    }

    m_textures.clear();
}