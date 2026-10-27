#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>

struct Sprite {
	SDL_Texture* texture=nullptr;
	SDL_Rect destRect{ 0,0,0,0 };
	SDL_Rect srcRect{ 0,0,0,0 };
	int zOrder = 0;
};

class RenderQueue
{
public:
	void Add(const Sprite& s) { m_sprites.emplace_back(s);}
	void Clear() { m_sprites.clear(); }
	void Flush(SDL_Renderer* renderer) {
		sort(m_sprites.begin(), m_sprites.end(), [](const Sprite& a, const Sprite& b) {
			return a.zOrder < b.zOrder;
		});
		for (const auto& sprite : m_sprites) {
			SDL_RenderCopy(renderer, sprite.texture,(sprite.srcRect.w ? &sprite.srcRect : nullptr), &sprite.destRect);
		}
		m_sprites.clear();
	}

private:
	std::vector<Sprite> m_sprites;

};

