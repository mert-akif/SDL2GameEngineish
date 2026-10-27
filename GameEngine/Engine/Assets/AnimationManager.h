#pragma once

#include <vector>
#include <deque>
#include <SDL2/SDL.h>
#include "../Core/Animation.h"

class AnimationManager
{
public:
    void CreateClip(
        SDL_Texture* texture,
        const std::string& textureKey,
        const std::string& texturePath,
        int animationLength,
        int animationId,
        int frameWidth,
        int frameHeight,
        float fps
    );

    const AnimationClip* GetClipById(int animationId) const;
    bool HasClip(int animationId) const;

    const std::deque<AnimationClip>& GetClips() const;

private:
    std::deque<AnimationClip> m_clips;
};