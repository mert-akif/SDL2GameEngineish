#include "AnimationManager.h"

void AnimationManager::CreateClip(
    SDL_Texture* texture,
    const std::string& textureKey,
    const std::string& texturePath,
    int animationLength,
    int animationId,
    int frameWidth,
    int frameHeight,
    float fps
)
{
    if (!texture)
        return;

    if (HasClip(animationId))
        return;

    AnimationClip clip;

    clip.id = animationId;
    clip.name = "Animation " + std::to_string(animationId);

    clip.texture = texture;
    clip.textureKey = textureKey;
    clip.texturePath = texturePath;

    clip.frameCount = animationLength;
    clip.frameWidth = frameWidth;
    clip.frameHeight = frameHeight;
    clip.fps = fps;

    for (int i = 0; i < animationLength; i++)
    {
        clip.frames.push_back({
            i * frameWidth,
            animationId * frameHeight,
            frameWidth,
            frameHeight
            });
    }

    m_clips.push_back(clip);
}

const AnimationClip* AnimationManager::GetClipById(int animationId) const
{
    for (const AnimationClip& clip : m_clips)
    {
        if (clip.id == animationId)
            return &clip;
    }

    return nullptr;
}

bool AnimationManager::HasClip(int animationId) const
{
    return GetClipById(animationId) != nullptr;
}

const std::deque<AnimationClip>& AnimationManager::GetClips() const
{
    return m_clips;
}