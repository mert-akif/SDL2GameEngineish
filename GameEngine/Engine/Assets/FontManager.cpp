#include "FontManager.h"


TTF_Font* FontManager::GetFont(const std::string& path, int ptSize)
{
	FontKey key{ path, ptSize };
	auto it = m_fonts.find(key);
	if (it != m_fonts.end()) {
		return it->second;
	}

	TTF_Font* font = TTF_OpenFont(path.c_str(), ptSize);
	if (!font) {
		std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
		return nullptr;
	}
	m_fonts[key] = font;
	return font;
}

SDL_Texture* FontManager::GetTextTexture(const std::string& text, const std::string& fontPath, int ptSize, SDL_Color color)
{
	const std::string cacheKey = text + "|" + fontPath + "|" + std::to_string(ptSize);
	auto it = m_fontCache.find(cacheKey);
	if (it != m_fontCache.end()) {
		return it->second;
	}
	TTF_Font* font = GetFont(fontPath, ptSize);
	if (!font) {
		return nullptr;
	}
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
	if (!surface) {
		std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
		return nullptr;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture) {
		std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
		return nullptr;
	}
	m_fontCache[cacheKey] = texture;
	return texture;
}

void FontManager::Shutdown()
{
	for (auto& pair : m_fonts) {
		TTF_CloseFont(pair.second);
	}
	m_fonts.clear();
	for (auto& pair : m_fontCache) {
		SDL_DestroyTexture(pair.second);
	}
	m_fontCache.clear();
	TTF_Quit();
} 
