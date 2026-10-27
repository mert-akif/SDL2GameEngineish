#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <iostream>

struct FontKey
{
	std::string name;
	int size;
};
inline bool operator==(const FontKey& a, const FontKey& b)
{
	return a.name == b.name && a.size == b.size;
}

struct FontKeyHash
{
	size_t operator() (const FontKey& fontKey) const noexcept {
		return std::hash<std::string>{}(fontKey.name) ^ (fontKey.size << 1);
	}
};

class FontManager {
	private:
		SDL_Renderer* m_renderer;
		std::unordered_map<FontKey, TTF_Font*, FontKeyHash> m_fonts;
		std::unordered_map<std::string, SDL_Texture*> m_fontCache;
	public:
		void Init(SDL_Renderer* renderer) {
			m_renderer = renderer;
			TTF_Init();
		};
		void Shutdown();
		TTF_Font* GetFont(const std::string& path, int ptSize);
		SDL_Texture* GetTextTexture(const std::string& text, const std::string& fontPath, int ptSize, SDL_Color color={255,255,255,255});

};
