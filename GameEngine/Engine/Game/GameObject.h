#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

#include"../Core/RenderQueue.h"
#include"../Core/PhysicsSystem.h"
#include"../Assets/AnimationManager.h"
#include"../Assets/AssetManager.h"
#include"Script.h"

class GameObject
{
public:
    int id = -1;
    std::string name = "GameObject";
    SDL_Texture* texture = nullptr;
    std::string textureKey = "m";
    std::string texturePath = "";

    // AnimationChecker
    bool hasAnimation = false;
    AnimationManager* animationManager = nullptr;
    AnimatedSprite animatedSprite;
    int animationId = -1;

    std::string animationTextureKey = "";
    std::string animationTexturePath = "";
    int animationFrameCount = 0;
    int animationFrameWidth = 32;
    int animationFrameHeight = 32;
    float animationFps = 12.0f;

    // Transform
    float x = 0.0f;
    float y = 0.0f;

    float width = 50.0f;
    float height = 50.0f;

    int orderInLayer = -1;

    // General
    bool active = true;

    // Physics
    bool usePhysics = false;
    bool isStatic = true;

    float mass = 1.0f;
    float gravityScale = 1.0f;

    float velocityX = 0.0f;
    float velocityY = 0.0f;

    // Collider
    bool hasCollider = true;
    bool isTrigger = false;

    //Rect
    SDL_Rect rect{ 0,0,0,0 };

    b2Body* m_body = nullptr;

    PhysicsSystem m_physys;

    // Script
    std::unique_ptr<Script> script;
    std::string scriptName = "";
    bool hasScript = false;

    float PPM;

    void Init(PhysicsSystem& physys, SDL_Texture* tex, AnimationManager* animaionManager, float ppm);

    void Update(float dt, ScriptContext& context);

    void Draw(RenderQueue& rq, int z);

    void RefreshAnimationSystem();
    void RefreshPosition();
    void RefreshSize();
    void RefreshVelocity();
    void RefreshBodyType();
    void RefreshSprite(AssetManager& assetManager);
    void PlayAnimation(int newAnimationId);
    void StopAnimation();

    void SetScript(std::unique_ptr<Script> newScript, ScriptContext& context);
    void RemoveScript(ScriptContext& context);
    void UpdateScript(float dt, ScriptContext& context);

};