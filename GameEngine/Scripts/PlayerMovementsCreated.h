#pragma once

#include "../Engine/Game/Script.h"
#include "../Engine/Game/GameObject.h"
#include "../Engine/Assets/SoundManager.h"
#include"../Engine/Core/ScriptContext.h"

class PlayerMovementsCreated : public Script
{
public:
    const char* GetName() const override
    {
        return "PlayerControllerScriptCreated";
    }

    void OnCreate(GameObject* owner, ScriptContext& context) override
    {
        if (!owner)
            return;

        owner->PlayAnimation(idleAnimationId);
    }

    void OnUpdate(GameObject* owner, ScriptContext& context, float deltaTime) override
    {
        if (!owner)
            return;

        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        bool moving = false;

        if (keys[SDL_SCANCODE_A])
        {
            owner->x -= speed * deltaTime;
            moving = true;
        }

        if (keys[SDL_SCANCODE_D])
        {
            owner->x += speed * deltaTime;
            moving = true;
        }

        if (keys[SDL_SCANCODE_W])
        {
            owner->y -= speed * deltaTime;
            moving = true;
        }

        if (keys[SDL_SCANCODE_S])
        {
            owner->y += speed * deltaTime;
            moving = true;
        }

        if (moving)
        {
            owner->PlayAnimation(walkAnimationId);
        }
        else
        {
            owner->PlayAnimation(idleAnimationId);
        }

        bool spacePressed = keys[SDL_SCANCODE_SPACE];

        if (spacePressed && !m_wasSpacePressed)
        {
            if (context.soundManager)
            {
                context.soundManager->PlaySoundEffect(jumpSoundId);
            }
        }

        m_wasSpacePressed = spacePressed;
        owner->RefreshPosition();
    }

private:
    float speed = 150.0f;

    int idleAnimationId = 0;
    int walkAnimationId = 1;

    int jumpSoundId = 0;

    bool m_wasSpacePressed = false;
};