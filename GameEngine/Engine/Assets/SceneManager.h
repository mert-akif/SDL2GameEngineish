#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include"../Game/GameObject.h"
#include"../Core/PhysicsSystem.h"
#include"AssetManager.h"
#include"AnimationManager.h"
#include"ScriptManager.h"
#include"SoundManager.h"

class SceneSerializer
{
public:
    static bool SaveScene(
        const std::string& path,
        const std::vector<GameObject>& gameObjects,
        const AssetManager& assetManager,
        const AnimationManager& animationManager,
        const SoundManager& soundManager
    );

    static bool LoadScene(
        const std::string& path,
        std::vector<GameObject>& gameObjects,
        PhysicsSystem& physicsSystem,
        AssetManager& assetManager,
        AnimationManager& animationManager,
        SoundManager& soundManager,
        ScriptRegistry& scriptRegistry,
		ScriptContext& scriptContext,
        float ppm,
        int& nextGameObjectId
    );
};