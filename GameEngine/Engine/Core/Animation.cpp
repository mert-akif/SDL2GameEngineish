#include "Animation.h"
#include "RenderQueue.h"

void AnimatedSprite::Update(float deltaTime)
{
	if (!m_isPlaying || !m_clip || m_clip->frames.empty()) return;
	m_frameTimer += deltaTime;
	float frameDuration = 1.0f / m_clip->fps;
	while (m_frameTimer >= frameDuration) {
		m_frameTimer -= frameDuration;
		m_currentFrame = (m_currentFrame + 1) % (int)m_clip->frames.size();
	}
}

void AnimatedSprite::Draw(RenderQueue& renderqueue,SDL_Rect* rect,int index)
{
	if (!m_clip || m_clip->frames.empty()) return;
	SDL_Rect srcRect = {static_cast<int> (rect->x),static_cast<int> (rect->y),m_clip->frames[0].w,m_clip->frames[0].h};
	renderqueue.Add({ m_clip->texture,srcRect,m_clip->frames[m_currentFrame],index });
}


