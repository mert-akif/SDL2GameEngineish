#pragma once

class AssetManager;
class AnimationManager;
class SoundManager;
class PhysicsSystem;

struct ScriptContext
{
    AssetManager* assetManager = nullptr;
    AnimationManager* animationManager = nullptr;
    SoundManager* soundManager = nullptr;
    PhysicsSystem* physicsSystem = nullptr;
};