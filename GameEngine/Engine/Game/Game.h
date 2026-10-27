#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <filesystem>
#include <SDL2/SDL_image.h>

#include "../Core/Animation.h"
#include "../Core/PhysicsSystem.h"
#include "../Core/RenderQueue.h"
#include "../Assets/AssetManager.h"
#include "../Assets/FontManager.h"
#include "../Assets/SoundManager.h"
#include "../../Vendor/IamGui/imgui.h"
#include "../../Vendor/IamGui/Backends/imgui_impl_sdl2.h"
#include "../../Vendor/IamGui/Backends/imgui_impl_sdlrenderer2.h"
#include "../Core/ImGuiLayer.h"
#include "../Assets/AnimationManager.h"
#include "../Assets/ScriptManager.h"
#include "../Assets/SceneManager.h"
#include "../Core/ScriptContext.h"

class Game
{
public:
	Game();
	~Game();
	bool Init(const std::string& title, int width, int height);
	void Run();
	void Shutdown();

	// Variables
private:

	// SDL variables
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	//imGui variables
	ImGuiLayer m_imguiLayer;

	//Physics system
	PhysicsSystem m_physicsSystem;

	struct vector2
	{
		float x = 0.0f, y = 0.0f;
	};

	//Render queue
	RenderQueue m_renderQueue;

	//Asset manager
	AssetManager m_assetManager;

	//Font manager
	FontManager m_fontManager;

	//Animations
	AnimationManager m_animationManager;


	//GameObject managing variables
	std::vector<GameObject> m_gameObjects;
	int m_selectedObjectIndex = -1;
	int m_nextGameObjectId = 0;

	//GameObjects
	GameObject player1;

	//Sound manager
	SoundManager m_soundManager;

	// Game state variables
	bool m_isRunning = false;
	bool m_isFullscreen = false;
	int m_windowWidth = 800;
	int m_windowHeight = 600;

	std::string m_lastDroppedTextureFilePath=" ";
	std::string m_lastDroppedSoundFilePath = " ";

	ScriptRegistry m_scriptRegistry;
	ScriptContext m_scriptContext;

#ifdef EDITOR_BUILD
	bool m_showDebugWindow = false;
	bool m_showSceneHierarchyWindow = true;
	bool m_showInspectorWindow = true;
	bool m_showSceneWindow = false;
	bool m_showBuildWindow = false;
	bool m_showSoundWindow = false;
	bool m_showAnimationLibraryWindow = false;
#endif


	//Functions
private:
	void ProcessInput();
	void HandleDroppedFile(const std::string& filePath);
	void UpdateEditor();
	void Update(float deltaTime);
	void Render();
};
