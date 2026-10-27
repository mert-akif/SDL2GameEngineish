#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_set>
#include <string>

struct AnimationClip
{
	int id = -1;
	std::string name = "Animation";

	SDL_Texture* texture = nullptr;
	std::string textureKey = "";
	std::string texturePath = "";

	std::vector<SDL_Rect> frames;

	int frameCount = 0;
	int frameWidth = 0;
	int frameHeight = 0;

	float fps = 12.0f;
};

class AnimatedSprite
{
private:
	const AnimationClip* m_clip = nullptr;
	int m_currentFrame = 0;
	float m_frameTimer = 0.0f;
	bool m_isPlaying = true;
public:
	AnimatedSprite() = default;
	void SetClip(const AnimationClip* clip) {
		m_clip = clip;
		m_currentFrame = 0;
		m_frameTimer = 0.0f;
	}
	void Update(float deltaTime);
	void Play() { m_isPlaying = true; }
	void Pause() { m_isPlaying = false; }
	void Draw(class RenderQueue& renderqueue,SDL_Rect* rect, int index);

	float x = 0.0f,y=0.0f;
};

class AnimationSystem
{
public:
	void Add(AnimatedSprite* sprite) {
		m_sprites.insert(sprite);
	}
	void Remove(AnimatedSprite* sprite) {
		m_sprites.erase(sprite);
	}
	void Update(float deltaTime) {
		for (AnimatedSprite* sprite : m_sprites) {
			sprite->Update(deltaTime);
		}
	}
	void Draw(RenderQueue& renderQueue, SDL_Rect* rect, int index) {
		for (AnimatedSprite* sprite : m_sprites) {
			sprite->Draw(renderQueue, rect, index);
		}
	}

private:
	std::unordered_set<AnimatedSprite*> m_sprites;

};
