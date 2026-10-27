#include "ImGuiLayer.h"

void ImGuiLayer::Init(SDL_Window* window, SDL_Renderer* renderer)
{
    m_renderer = renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void ImGuiLayer::ProcessEvent(SDL_Event& event)
{
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void ImGuiLayer::BeginFrame()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::MainMenuBar(
    bool& showDebugWindow,
    bool& showSceneHierarchyWindow,
    bool& showInspectorWindow,
    bool& showSceneWindow,
    bool& showBuildWindow,
    bool& showSoundWindow,
    bool& showAnimationLibraryWindow
)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Scene Save / Load"))
            {
                showSceneWindow = true;
            }

            if (ImGui::MenuItem("Build Game"))
            {
                showBuildWindow = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Close All Windows"))
            {
                showDebugWindow = false;
                showSceneHierarchyWindow = false;
                showInspectorWindow = false;
                showSceneWindow = false;
                showBuildWindow = false;
                showSoundWindow = false;
                showAnimationLibraryWindow = false;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            ImGui::MenuItem("Scene Hierarchy", nullptr, &showSceneHierarchyWindow);
            ImGui::MenuItem("Inspector", nullptr, &showInspectorWindow);
            ImGui::MenuItem("Scene Save / Load", nullptr, &showSceneWindow);
            ImGui::MenuItem("Animation Library", nullptr, &showAnimationLibraryWindow);
            ImGui::MenuItem("Sound Manager", nullptr, &showSoundWindow);
            ImGui::MenuItem("Build", nullptr, &showBuildWindow);
            ImGui::MenuItem("Debug", nullptr, &showDebugWindow);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Layout"))
        {
            if (ImGui::MenuItem("Default Layout"))
            {
                showSceneHierarchyWindow = true;
                showInspectorWindow = true;

                showDebugWindow = false;
                showSceneWindow = false;
                showBuildWindow = false;
                showSoundWindow = false;
                showAnimationLibraryWindow = false;
            }

            if (ImGui::MenuItem("Full Editor Layout"))
            {
                showSceneHierarchyWindow = true;
                showInspectorWindow = true;
                showSceneWindow = true;
                showAnimationLibraryWindow = true;
                showSoundWindow = true;
                showBuildWindow = true;
                showDebugWindow = true;
            }

            if (ImGui::MenuItem("Close All"))
            {
                showDebugWindow = false;
                showSceneHierarchyWindow = false;
                showInspectorWindow = false;
                showSceneWindow = false;
                showBuildWindow = false;
                showSoundWindow = false;
                showAnimationLibraryWindow = false;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void ImGuiLayer::DebugPage(float* ppm, std::vector<GameObject>& gameObjects)
{
    ImGui::Begin("Debug");

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Delta Time: %.3f ms", ImGui::GetIO().DeltaTime);
    ImGui::Text("Objects: %d", gameObjects.size());
    ImGui::SliderFloat("PPM", ppm, 1.f, 100.f);

    static bool showDemo = false;
    ImGui::Checkbox("Show ImGui Demo", &showDemo);

    if (showDemo)
        ImGui::ShowDemoWindow(&showDemo);

    ImGui::End();
}

void ImGuiLayer::SceneHierarchyPage(
    std::vector<GameObject>& gameObjects,
    int& selectedObjectIndex,
    int& nextGameObjectId,
    PhysicsSystem& physys,
    float ppm,
    AssetManager& assetManager,
    AnimationManager& animationManager


)
{
    ImGui::Begin("Scene Hierarchy");

    for (GameObject& object : gameObjects)
    {
        std::string label = object.name + "##" + std::to_string(object.id);

        bool isSelected = selectedObjectIndex == object.id;

        if (ImGui::Selectable(label.c_str(), isSelected))
        {
            selectedObjectIndex = object.id;
        }
    }

    if (ImGui::Button("Create Empty Object"))
    {
        GameObject obj;
        obj.id = nextGameObjectId++;
        obj.name = "Empty Object" + std::to_string(obj.id);
        obj.x = 100.0f;
        obj.y = 100.0f;
        obj.textureKey = "p";
        obj.Init(physys, assetManager.LoadTexture(obj.textureKey,obj.texturePath), &animationManager, ppm);

        gameObjects.push_back(std::move(obj));

        selectedObjectIndex = obj.id;
    }

    if (selectedObjectIndex != -1)
    {
        if (ImGui::Button("Delete Selected Object"))
        {
            gameObjects.erase(
                std::remove_if(
                    gameObjects.begin(),
                    gameObjects.end(),
                    [selectedObjectIndex](const GameObject& object)
                    {
                        return object.id == selectedObjectIndex;
                    }
                ),
                gameObjects.end()
            );

            selectedObjectIndex = -1;
        }
    }

    ImGui::End();
}

void ImGuiLayer::InspectorPage(
    std::vector<GameObject>& gameObjects,
    int selectedObjectId,
    PhysicsSystem& physys,
    AssetManager& assetManager,
    AnimationManager& animationManager,
    const std::string lastDroppedFilePath,
	ScriptRegistry& scriptRegistry,
	ScriptContext& scriptContext
)
{
    ImGui::Begin("Inspector");

    GameObject* selectedObject = nullptr;

    for (GameObject& object : gameObjects)
    {
        if (object.id == selectedObjectId)
        {
            selectedObject = &object;
            break;
        }
    }

    if (!selectedObject)
    {
        ImGui::Text("No object selected.");
        ImGui::End();
        return;
    }

    ImGui::Text("ID: %d", selectedObject->id);

    char nameBuffer[128];
    strcpy_s(nameBuffer, selectedObject->name.c_str());

    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
    {
        selectedObject->name = nameBuffer;
    }

    ImGui::Checkbox("Active", &selectedObject->active);

    if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Has Animation", &selectedObject->hasAnimation);

        const auto& clips = animationManager.GetClips();

        if (selectedObject->hasAnimation)
        {
            std::string currentLabel = "None";

            const AnimationClip* currentClip =
                animationManager.GetClipById(selectedObject->animationId);

            if (currentClip)
            {
                currentLabel =
                    currentClip->name + " (ID: " + std::to_string(currentClip->id) + ")";
            }

            if (ImGui::BeginCombo("Animation Clip", currentLabel.c_str()))
            {
                for (const AnimationClip& clip : clips)
                {
                    bool isSelected = selectedObject->animationId == clip.id;

                    std::string label =
                        clip.name + " (ID: " + std::to_string(clip.id) + ")";

                    if (ImGui::Selectable(label.c_str(), isSelected))
                    {
                        selectedObject->hasAnimation = true;
                        selectedObject->animationId = clip.id;
                        selectedObject->animationManager = &animationManager;

                        selectedObject->animationTextureKey = clip.textureKey;
                        selectedObject->animationTexturePath = clip.texturePath;

                        selectedObject->animationFrameCount = clip.frameCount;
                        selectedObject->animationFrameWidth = clip.frameWidth;
                        selectedObject->animationFrameHeight = clip.frameHeight;
                        selectedObject->animationFps = clip.fps;

                        selectedObject->RefreshAnimationSystem();
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            if (currentClip)
            {
                ImGui::Text("Texture Key: %s", currentClip->textureKey.c_str());
                ImGui::Text("Texture Path: %s", currentClip->texturePath.c_str());
                ImGui::Text("Frames: %d", currentClip->frameCount);
                ImGui::Text("Frame Size: %d x %d", currentClip->frameWidth, currentClip->frameHeight);
                ImGui::Text("FPS: %.1f", currentClip->fps);
            }
        }
    }
    if (selectedObject->hasAnimation)
    {
        selectedObject->textureKey = "w";
        selectedObject->texturePath = assetManager.GetTexturePath("w");
        selectedObject->texture = assetManager.GetTexture("w");
    }
    else {
        if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static char textureKeyBuffer[128] = "";
            static int lastSelectedObjectId = -1;

            if (lastSelectedObjectId != selectedObject->id)
            {
                strcpy_s(textureKeyBuffer, selectedObject->textureKey.c_str());
                lastSelectedObjectId = selectedObject->id;
            }

            const std::unordered_map<std::string, TextureAsset>& textures = assetManager.GetTextures();

            std::string currentTextureLabel = selectedObject->textureKey.empty()
                ? "None"
                : selectedObject->textureKey;



            if (ImGui::BeginCombo("Sprite Texture", currentTextureLabel.c_str()))
            {
                for (const auto& pair : textures)
                {
                    const std::string& key = pair.first;
                    TextureAsset texture = pair.second;

                    bool isSelected = selectedObject->textureKey == key;

                    std::string label = key + "##texture_" + key;

                    if (ImGui::Selectable(label.c_str(), isSelected))
                    {
                        selectedObject->textureKey = key;
                        selectedObject->texturePath = assetManager.GetTexturePath(key);
                        selectedObject->texture = assetManager.GetTexture(key);

                        strcpy_s(textureKeyBuffer, selectedObject->textureKey.c_str());
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }




        }
    }

    

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::DragFloat("Position X", &selectedObject->x, 1.0f) || ImGui::DragFloat("Position Y", &selectedObject->y, 1.0f)) {
            selectedObject->RefreshPosition();
        }

        if (ImGui::DragFloat("Width", &selectedObject->width, 1.0f, 1.0f, 1000.0f) || ImGui::DragFloat("Height", &selectedObject->height, 1.0f, 1.0f, 1000.0f)) {
            selectedObject->RefreshSize();
        }

        ImGui::DragInt("OrderInLayer", &selectedObject->orderInLayer, 0, 100);
    }

    if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Checkbox("Use Physics", &selectedObject->usePhysics)) {
            selectedObject->RefreshBodyType();
        }

        if (selectedObject->usePhysics)
        {


            if (ImGui::Checkbox("Is Static", &selectedObject->isStatic)) {
                selectedObject->RefreshBodyType();
            }

            if (ImGui::DragFloat("Velocity X", &selectedObject->velocityX, 0.1f) || ImGui::DragFloat("Velocity Y", &selectedObject->velocityY, 0.1f)) {
                selectedObject->RefreshVelocity();
            }

            if (!selectedObject->isStatic)
            {
                ImGui::DragFloat("Mass", &selectedObject->mass, 0.1f, 0.1f, 1000.0f);
                ImGui::DragFloat("Gravity Scale", &selectedObject->gravityScale, 0.1f, 0.0f, 10.0f);
            }
            else
            {
                ImGui::TextDisabled("Static objects are not affected by velocity or gravity.");
            }
        }
        else
        {
            ImGui::TextDisabled("Physics disabled for this object.");
        }

    }

    if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Has Collider", &selectedObject->hasCollider);

        if (selectedObject->hasCollider)
        {
            ImGui::Checkbox("Is Trigger", &selectedObject->isTrigger);
        }
    }

	ImGui::Separator();

    if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (selectedObject->script)
        {
            ImGui::Text("Current Script: %s", selectedObject->script->GetName());

            if (ImGui::Button("Remove Script"))
            {
                selectedObject->RemoveScript(scriptContext);
            }
        }
        else
        {
            ImGui::TextDisabled("No script attached.");
        }

        ImGui::Separator();

        const auto& scripts = scriptRegistry.GetScripts();

        static int selectedScriptIndex = 0;

        std::string currentScriptLabel = "Select Script";

        if (!scripts.empty() &&
            selectedScriptIndex >= 0 &&
            selectedScriptIndex < static_cast<int>(scripts.size()))
        {
            currentScriptLabel = scripts[selectedScriptIndex].name;
        }

        if (ImGui::BeginCombo("Registered Scripts", currentScriptLabel.c_str()))
        {
            for (int i = 0; i < static_cast<int>(scripts.size()); i++)
            {
                bool isSelected = selectedScriptIndex == i;

                if (ImGui::Selectable(scripts[i].name.c_str(), isSelected))
                {
                    selectedScriptIndex = i;
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Add Selected Script"))
        {
            if (!scripts.empty())
            {
                std::unique_ptr<Script> newScript =
                    scriptRegistry.CreateScript(scripts[selectedScriptIndex].name);

                selectedObject->SetScript(std::move(newScript),scriptContext);
            }
        }

        ImGui::Separator();

        static char newScriptNameBuffer[128] = "NewScript";
        static bool scriptCreated = false;
        static bool scriptCreateFailed = false;

        ImGui::InputText("New Script Name", newScriptNameBuffer, sizeof(newScriptNameBuffer));

        if (ImGui::Button("Create Script File"))
        {
            scriptCreated = false;
            scriptCreateFailed = false;

            bool result = ScriptGenerator::CreateScriptFile(
                newScriptNameBuffer,
                "Scripts",
                "Engine/Assets/ScriptManager.h"
            );

            if (result)
            {
                scriptCreated = true;
            }
            else
            {
                scriptCreateFailed = true;
            }
        }

        if (scriptCreated)
        {
            ImGui::TextColored(
                ImVec4(0.3f, 1.0f, 0.3f, 1.0f),
                "Script file created. Include and register it, then rebuild."
            );
        }

        if (scriptCreateFailed)
        {
            ImGui::TextColored(
                ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
                "Failed to create script. It may already exist."
            );
        }

        ImGui::TextDisabled("Note: C++ scripts require rebuild before they can be attached.");
    }



    ImGui::End();
}

void ImGuiLayer::AssetBrowserPage(AssetManager& assetManager, AnimationManager& animationManager, const std::string& lastDroppedFilePath)
{
    ImGui::Begin("Asset Browser");

    if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static char textureKeyBuffer[128] = "";
        static char texturePathBuffer[256] = "";

		static bool isAnimationTexture=false;

        static int lastSelectedObjectId = -1;
        static std::string lastCopiedDroppedPath = "";

        if (!lastDroppedFilePath.empty() && lastDroppedFilePath != lastCopiedDroppedPath)
        {
            strcpy_s(texturePathBuffer, lastDroppedFilePath.c_str());
            lastCopiedDroppedPath = lastDroppedFilePath;
        }

        ImGui::Checkbox("IsAnimationTexture", &isAnimationTexture);

        if (isAnimationTexture)
        {
			strcpy_s(textureKeyBuffer,sizeof(textureKeyBuffer), "w");
        }
        else 
        {
            ImGui::InputText("Texture Key", textureKeyBuffer, sizeof(textureKeyBuffer));
        }

        if (ImGui::InputText("Texture Path", texturePathBuffer, sizeof(texturePathBuffer))) {
            lastCopiedDroppedPath = "";
        }

        if (!lastDroppedFilePath.empty())
        {
            ImGui::TextWrapped("Last dropped: %s", lastDroppedFilePath.c_str());
        }

        if (ImGui::Button("Create Texture"))
        {
            if (!lastDroppedFilePath.empty())
            {
                assetManager.RemoveTexture(textureKeyBuffer);

                SDL_Texture* tex = assetManager.LoadTexture(
                    textureKeyBuffer,
                    texturePathBuffer
                );

            }
        }

		if (ImGui::Button("Clear Texture"))
		{
            assetManager.RemoveTexture(textureKeyBuffer);
		}



    }
    if (ImGui::CollapsingHeader("Create Animation Clip"))
    {
        static char textureKeyBuffer[128] = "w";
		std::string texturePath = "";

        static int newAnimationId = 0;
        static int animationLength = 16;
        static int frameWidth = 32;
        static int frameHeight = 32;
        static float fps = 12.0f;

        //ImGui::InputText("Animation Texture Key", textureKeyBuffer, sizeof(textureKeyBuffer));
        
        texturePath = assetManager.GetTexturePath(textureKeyBuffer);

        ImGui::InputInt("Animation ID / Row", &newAnimationId);
        ImGui::InputInt("Frame Count", &animationLength);
        ImGui::InputInt("Frame Width", &frameWidth);
        ImGui::InputInt("Frame Height", &frameHeight);
        ImGui::DragFloat("FPS", &fps, 0.1f, 1.0f, 60.0f);

        if (animationLength < 1) animationLength = 1;
        if (frameWidth < 1) frameWidth = 1;
        if (frameHeight < 1) frameHeight = 1;
        if (fps < 1.0f) fps = 1.0f;

        if (animationManager.HasClip(newAnimationId))
        {
            ImGui::TextDisabled("A clip with this ID already exists.");
        }

        if (ImGui::Button("Create Clip"))
        {
            SDL_Texture* tex = assetManager.LoadTexture(textureKeyBuffer, texturePath);

            if (tex)
            {
                animationManager.CreateClip(
                    tex,
                    textureKeyBuffer, 
                    texturePath,
                    animationLength,
                    newAnimationId,
                    frameWidth,
                    frameHeight,
                    fps
                );

            }
        }

    }

    ImGui::End();
}

void ImGuiLayer::SoundPage(SoundManager& soundManager,const std::string& lastDroppedFilePath)
{
    ImGui::Begin("Sound Manager");

    static char soundNameBuffer[128] = "";
    static char soundPathBuffer[256] = "";

    static std::string lastCopiedDroppedPath = "";

    ImGui::InputText("Sound Name", soundNameBuffer, sizeof(soundNameBuffer));

    ImGui::InputText("Sound Path", soundPathBuffer, sizeof(soundPathBuffer));

    if (!lastDroppedFilePath.empty() && lastDroppedFilePath != lastCopiedDroppedPath)
    {
        strcpy_s(soundPathBuffer, lastDroppedFilePath.c_str());
        lastCopiedDroppedPath = lastDroppedFilePath;
    }

    if (ImGui::Button("Load Sound Effect"))
    {
        soundManager.LoadSoundEffect(
            soundNameBuffer,
            soundPathBuffer
        );
    }

    ImGui::SameLine();

    if (ImGui::Button("Load Music"))
    {
        soundManager.LoadMusic(
            soundNameBuffer,
            soundPathBuffer
        );
    }

    ImGui::Separator();

    ImGui::Text("Sound Effects");

    for (const auto& pair : soundManager.GetSoundEffects())
    {
        const SoundEffectAsset& sound = pair.second;

        ImGui::Text(
            "ID: %d | %s",
            sound.id,
            sound.name.c_str()
        );

        ImGui::SameLine();

        std::string playButton = "Play##sfx_" + std::to_string(sound.id);

        if (ImGui::Button(playButton.c_str()))
        {
            soundManager.PlaySoundEffect(sound.id);
        }

        ImGui::SameLine();

        std::string removeButton = "Remove##sfx_" + std::to_string(sound.id);

        if (ImGui::Button(removeButton.c_str()))
        {
            soundManager.RemoveSoundEffect(sound.id);
            break;
        }
    }

    ImGui::Separator();

    ImGui::Text("Music");

    for (const auto& pair : soundManager.GetMusicAssets())
    {
        const MusicAsset& music = pair.second;

        ImGui::Text(
            "ID: %d | %s",
            music.id,
            music.name.c_str()
        );

        ImGui::SameLine();

        std::string playButton = "Play##music_" + std::to_string(music.id);

        if (ImGui::Button(playButton.c_str()))
        {
            soundManager.PlayMusic(music.id);
        }

        ImGui::SameLine();

        std::string removeButton = "Remove##music_" + std::to_string(music.id);

        if (ImGui::Button(removeButton.c_str()))
        {
            soundManager.RemoveMusic(music.id);
            break;
        }
    }

    if (ImGui::Button("Stop Music"))
    {
        soundManager.StopMusic();
    }

    ImGui::End();
}

void ImGuiLayer::BuildPage()
{
    ImGui::Begin("Build");

    ImGui::Text("Build Tools");

    const char* buildDirectory = "Build";

    if (ImGui::Button("Build Debug / Editor"))
    {
        ShellExecuteA(
            nullptr,
            "open",
            "build-debug.bat",
            nullptr,
            buildDirectory,
            SW_SHOWNORMAL
        );
    }

    if (ImGui::Button("Build Release / Game"))
    {
        ShellExecuteA(
            nullptr,
            "open",
            "build-release.bat",
            nullptr,
            buildDirectory,
            SW_SHOWNORMAL
        );
    }

    if (ImGui::Button("Run Debug / Editor"))
    {
        ShellExecuteA(
            nullptr,
            "open",
            "run-debug.bat",
            nullptr,
            buildDirectory,
            SW_SHOWNORMAL
        );
    }

    if (ImGui::Button("Clean Build Files"))
    {
        ShellExecuteA(
            nullptr,
            "open",
            "clean.bat",
            nullptr,
            buildDirectory,
            SW_SHOWNORMAL
        );
    }

    ImGui::End();
}

void ImGuiLayer::ScenePage(
    std::vector<GameObject>& gameObjects,
    PhysicsSystem& physicsSystem,
    AssetManager& assetManager,
    AnimationManager& animationManager,
	ScriptRegistry& scriptRegistry,
	SoundManager& soundManager,
	ScriptContext& scriptContext,
    float ppm,
    int& nextGameObjectId
)
{
    ImGui::Begin("Scene");

    if (ImGui::Button("Save Scene"))
    {
        SceneSerializer::SaveScene(
            "Assets/Scenes/Main.scene",
            gameObjects,
            assetManager,
			animationManager,
            soundManager
        );
    }

    if (ImGui::Button("Load Scene"))
    {
        SceneSerializer::LoadScene(
            "Assets/Scenes/Main.scene",
            gameObjects,
            physicsSystem,
            assetManager,
            animationManager,
			soundManager,
			scriptRegistry,
            scriptContext,
            ppm,
            nextGameObjectId
        );
    }

    ImGui::End();
}

void ImGuiLayer::Render()
{
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
}

void ImGuiLayer::Shutdown()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}