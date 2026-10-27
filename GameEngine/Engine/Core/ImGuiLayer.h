#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <Windows.h>

#include"../../Vendor/IamGui/imgui.h"
#include"../../Vendor/IamGui/Backends/imgui_impl_sdl2.h"
#include"../../Vendor/IamGui/Backends/imgui_impl_sdlrenderer2.h"
#include"../Game/GameObject.h"
#include"PhysicsSystem.h"
#include "../Assets/AssetManager.h"
#include "../Assets/ScriptManager.h"
#include"../Assets/SceneManager.h"
#include"../Assets/SoundManager.h"

class ImGuiLayer
{
public:
    void Init(SDL_Window* window, SDL_Renderer* renderer);
    void ProcessEvent(SDL_Event& event);
    void BeginFrame();

    void MainMenuBar(
        bool& showDebugWindow,
        bool& showSceneHierarchyWindow,
        bool& showInspectorWindow,
        bool& showSceneWindow,
        bool& showBuildWindow,
        bool& showSoundWindow,
        bool& showAnimationLibraryWindow
    );

    void DebugPage(float* ppm, std::vector<GameObject>& gameObjects);

    void SceneHierarchyPage(
        std::vector<GameObject>& gameObjects,
        int& selectedObjectIndex,
        int& nextGameObjectId,
        PhysicsSystem& physys,
        float ppm,
        AssetManager& assetManager,
        AnimationManager& animationManager
    );

    void InspectorPage(    
        std::vector<GameObject>& gameObjects,
        int selectedObjectId,
        PhysicsSystem& physys,
        AssetManager& assetManager,
        AnimationManager& animationManager,
        std::string lastDroppedFilePath,
		ScriptRegistry& scriptRegistry,
        ScriptContext& scriptContext
    );

    void AssetBrowserPage(AssetManager& assetManager,AnimationManager& animationManager, const std::string& lastDroppedFilePath);

    void SoundPage(SoundManager& soundManager,const std::string& lastDroppedFilePath);

    void ScenePage(
        std::vector<GameObject>& gameObjects,
        PhysicsSystem& physicsSystem,
        AssetManager& assetManager,
        AnimationManager& animationManager,
        ScriptRegistry& scriptRegistry,
		SoundManager& soundManager,
        ScriptContext& scriptContext,
        float ppm,
        int& nextGameObjectId
    );

    void BuildPage();

 

    void Render();
    void Shutdown();


private:
    SDL_Renderer* m_renderer = nullptr;
};