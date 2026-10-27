#include "Game.h";

Game::Game()
{
}
Game::~Game()
{
	Shutdown();
}
constexpr int MAX_FPS = 60;
constexpr int FRAME_DELAY = 1000/MAX_FPS;
float PPM = 32.f; // Pixels Per Meter for physics scaling

bool Game::Init(const std::string& title, int width, int height)
{
#ifdef EDITOR_BUILD
	std::cout << "EDITOR BUILD ACTIVE" << std::endl;
#else
	std::cout << "GAME BUILD ACTIVE" << std::endl;
#endif

	// Initialize physics system
	m_physicsSystem.Init(PPM, 0.0f, 0.0f);

	// Set window properties
	m_windowWidth = width;
	m_windowHeight = height;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return false;
	}

	// Create the window
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, SDL_WINDOW_SHOWN);
	if (!m_window) {
		std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
		return false;
	}

	// Create the renderer
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!m_renderer) {
		std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
		return false;
	}


	#ifdef EDITOR_BUILD
		m_imguiLayer.Init(m_window, m_renderer);
	#endif // EDITOR_BUILD


	
	//Texture loading
	m_assetManager.Init(m_renderer);

	SDL_Texture* texture=m_assetManager.LoadTexture("p", "Assets/Textures/brick.png");

	//Animation System
	//SDL_Texture* animtexture = m_assetManager.LoadTexture("m", "Assets/Textures/missing.png");



	////Player initialization
	//SDL_Texture* playerTexture= m_assetManager.LoadTexture("player","Assets/Textures/brick.png");
	//m_playerL.Init(m_physicsSystem, 20, m_windowHeight * 0.5f);
	//m_playerR.Init(m_physicsSystem, m_windowWidth - 20, m_windowHeight * 0.5f);

	////Ball initialization
	//m_ball.Init(m_physicsSystem, m_windowWidth * 0.5f, m_windowHeight * 0.5f, 10.f);
	//m_ball.Launch(300.f, 150.f);

	//Sound manager
	m_soundManager.Init();
	//m_soundManager.GetSound("GameOver", "Assets/Sounds/GameOver.mp3");
	//m_soundManager.PlaySound("GameOver");

	//Font manager
	m_fontManager.Init(m_renderer);

	m_scriptContext.animationManager = &m_animationManager;
	m_scriptContext.assetManager = &m_assetManager;
	m_scriptContext.soundManager = &m_soundManager;
	m_scriptContext.physicsSystem = &m_physicsSystem;

	// Walls
	//m_wallTop = m_physicsSystem.CreateBox(m_windowWidth / 2.f, 5.f, m_windowWidth, 10.f, false);
	//m_wallBottom = m_physicsSystem.CreateBox(m_windowWidth / 2.f, m_windowHeight - 5.f, m_windowWidth, 10.f, false);

	#ifndef EDITOR_BUILD
		SceneSerializer::LoadScene(
			"Assets/Scenes/Main.scene",
			m_gameObjects,
			m_physicsSystem,
			m_assetManager,
			m_animationManager,
			m_soundManager,
			m_scriptRegistry,
			m_scriptContext,
			PPM,
			m_nextGameObjectId
		);
	#endif

	// Set the running flag
	m_isRunning = true;
	return true;
}

void Game::HandleDroppedFile(const std::string& filePath)
{
	std::filesystem::path path(filePath);
	std::string extension = path.extension().string();

	for (char& c : extension)
		c = static_cast<char>(std::tolower(c));

	if (extension != ".png" && extension != ".jpg" && extension != ".jpeg" && extension != ".bmp" && extension != ".mp3")
	{
		std::cout << "Unsupported file type: " << extension << std::endl;
		return;
	}
		if (extension == ".mp3") {
			m_lastDroppedSoundFilePath = filePath;
		}

		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp") {
			m_lastDroppedTextureFilePath = filePath;
		}
		
	


	//GameObject* selectedObject = nullptr;

	//for (GameObject& object : m_gameObjects)
	//{
	//	if (object.id == m_selectedObjectIndex)
	//	{
	//		selectedObject = &object;
	//		break;
	//	}
	//}

	//if (!selectedObject)
	//{
	//	std::cout << "No selected object." << std::endl;
	//	return;
	//}

	//std::string key = path.stem().string() + "_" + std::to_string(selectedObject->id);

	//SDL_Texture* texture = m_assetManager.LoadTexture(key, filePath);

	//if (!texture)
	//{
	//	std::cout << "Failed to load texture: " << filePath << std::endl;
	//	return;
	//}

	//selectedObject->textureKey = key;
	//selectedObject->texture = texture;

	//std::cout << "Texture assigned: " << key << std::endl;
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// Pass event to ImGui
		#ifdef EDITOR_BUILD
				m_imguiLayer.ProcessEvent(event);
		#endif // EDITOR_BUILD



		// Handle events
		switch (event.type) {
			case SDL_QUIT:
				m_isRunning = false;
				break;

			case SDL_DROPFILE:
			{
				char* droppedFilePath = event.drop.file;

				std::cout << "Dropped file: " << droppedFilePath << std::endl;

				HandleDroppedFile(droppedFilePath);

				SDL_free(droppedFilePath);
				break;
			}

			//case SDL_KEYDOWN:
			//	cout << "Key pressed: " << SDL_GetKeyName(event.key.keysym.sym) << endl;
			//	break;

			//case SDL_KEYUP:
			//	cout << "Key released: " << SDL_GetKeyName(event.key.keysym.sym) << endl;

			//	break;

			//case SDL_MOUSEBUTTONDOWN:
			//	cout << "Mouse button pressed: " << (int)event.button.button << " at (" << event.button.x << ", " << event.button.y << ")" << endl;
			//	break;

			//case SDL_MOUSEBUTTONUP:
			//	cout << "Mouse button released: " << (int)event.button.button << " at (" << event.button.x << ", " << event.button.y << ")" << endl;
			//	break;

			//case SDL_MOUSEMOTION:
			//	cout << "Mouse moved to: (" << event.motion.x << ", " << event.motion.y << ")" << endl;

			//	break;

			//case SDL_MOUSEWHEEL:break;

			finally:
			break;

		}


	}
}
#ifdef EDITOR_BUILD
void Game::UpdateEditor()
{
	m_imguiLayer.BeginFrame();

	m_imguiLayer.MainMenuBar(
		m_showDebugWindow,
		m_showSceneHierarchyWindow,
		m_showInspectorWindow,
		m_showSceneWindow,
		m_showBuildWindow,
		m_showSoundWindow,
		m_showAnimationLibraryWindow
	);

	if (m_showDebugWindow)
	{
		m_imguiLayer.DebugPage(
			&PPM,
			m_gameObjects
		);
	}

	if (m_showSceneHierarchyWindow)
	{
		m_imguiLayer.SceneHierarchyPage(
			m_gameObjects,
			m_selectedObjectIndex,
			m_nextGameObjectId,
			m_physicsSystem,
			PPM,
			m_assetManager,
			m_animationManager
		);
	}

	if (m_showInspectorWindow)
	{
		m_imguiLayer.InspectorPage(
			m_gameObjects,
			m_selectedObjectIndex,
			m_physicsSystem,
			m_assetManager,
			m_animationManager,
			m_lastDroppedTextureFilePath,
			m_scriptRegistry,
			m_scriptContext
		);

	}

	if (m_showSceneWindow)
	{
		m_imguiLayer.ScenePage(
			m_gameObjects,
			m_physicsSystem,
			m_assetManager,
			m_animationManager,
			m_scriptRegistry,
			m_soundManager,
			m_scriptContext,
			PPM,
			m_nextGameObjectId
		);
	}

	if (m_showBuildWindow)
	{
		m_imguiLayer.BuildPage();
	}

	if (m_showSoundWindow)
	{
		m_imguiLayer.SoundPage(m_soundManager, m_lastDroppedSoundFilePath);
	}

	if (m_showAnimationLibraryWindow)
	{
		m_imguiLayer.AssetBrowserPage(m_assetManager, m_animationManager, m_lastDroppedTextureFilePath);
	}
}
#endif

void Game::Update(float deltaTime)
{
	m_physicsSystem.Step(deltaTime);

	for (GameObject& object : m_gameObjects) {
		object.Update(deltaTime,m_scriptContext);
	}

	//m_animationManager.Update(deltaTime);

	//const Uint8* keys = SDL_GetKeyboardState(nullptr);
	//m_playerL.Update(keys, deltaTime, SDL_SCANCODE_W, SDL_SCANCODE_S, 300.f,0.f,static_cast<float>(m_windowHeight));
	//m_playerR.Update(keys, deltaTime, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, 300.f, 0.f, static_cast<float>(m_windowHeight));

	//m_ball.Update();


	//b2Vec2 pos = m_ball.Body()->GetPosition();
	//if (pos.x * PPM < 0) {
	//	m_scoreR++;
	//	m_ball.Reset(m_windowWidth / 2.f, m_windowHeight / 2.f);
	//	m_ball.Launch(300.f, -150.f);
	//}
	//else if (pos.x * PPM > m_windowWidth) {
	//	m_scoreL++;
	//	m_ball.Reset(m_windowWidth / 2.f, m_windowHeight / 2.f);
	//	m_ball.Launch(-300.f, 150.f);
	//}

	//if (m_ball.Body()->GetLinearVelocity().x<0.1f&& m_ball.Body()->GetLinearVelocity().x > -0.1f) {
	//	m_ball.Launch(300.f, 150.f);
	//}
	//if (m_ball.Body()->GetLinearVelocity().y<0.1f && m_ball.Body()->GetLinearVelocity().y> -0.1f) {
	//	m_ball.Launch(-300.f, -150.f);
	//}

	//SDL_Color col{ 255, 255, 255, 255 };

	//m_scoreTextureL = m_fontManager.GetTextTexture(std::to_string(m_scoreL), "Assets/Fonts/OpenSans-Regular.ttf", 24, col);
	//m_scoreTextureR = m_fontManager.GetTextTexture(std::to_string(m_scoreR), "Assets/Fonts/OpenSans-Regular.ttf", 24, col);
}

void Game::Render()
{
	// Clear the screen
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);
	m_renderQueue.Clear();

	//m_animationManager.Draw(m_renderQueue,10);

	//SDL_Texture* playerTexture = m_assetManager.LoadTexture("player");

	//m_playerL.Draw(m_renderQueue, playerTexture, 0);
	//m_playerR.Draw(m_renderQueue, playerTexture, 0);

	for (GameObject& object : m_gameObjects) {
		object.Draw(m_renderQueue,object.orderInLayer);
	}

	// Flush the render queue
	m_renderQueue.Flush(m_renderer);

	char a = 'a';
	char b = 'b';

	#ifdef EDITOR_BUILD
		m_imguiLayer.Render();
	#endif // EDITOR_BUILD

	//SDL_Texture* brickTex = m_assetManager.LoadTexture("ball", "Assets/Textures/heart.png");
	//m_ball.Draw(m_renderQueue, brickTex, 0);

	//if (m_scoreTextureL && m_scoreTextureL) {
	//	int w1, h1;
	//	int w2, h2;

	//	SDL_QueryTexture(m_scoreTextureL, nullptr, nullptr, &w1, &h1);
	//	SDL_QueryTexture(m_scoreTextureR, nullptr, nullptr, &w2, &h2);

	//	SDL_Rect dl{ m_windowWidth / 4 - w1 / 2, 20, w1, h1 };
	//	SDL_Rect dr{ 3 * m_windowWidth / 4 - w2 / 2, 20, w2, h2 };
	//	m_renderQueue.Add({ m_scoreTextureL, dl, SDL_Rect{0,0,0,0}, 1 });
	//	m_renderQueue.Add({ m_scoreTextureR, dr, SDL_Rect{0,0,0,0}, 1 });
	// 
	//}

	m_renderQueue.Flush(m_renderer);


	SDL_RenderPresent(m_renderer); // Render et - Double-buffering

}

void Game::Run()
{
	Uint64 frameStartTime=SDL_GetPerformanceCounter();
	Uint64 frequency = SDL_GetPerformanceFrequency();


	Uint32 frameStart;
	Uint32 elapsedTime;
	Uint32 fpsTimer = SDL_GetTicks();
	int frameCount = 0;

	while (m_isRunning) {
		
		Uint64 currentTime = SDL_GetPerformanceCounter();
		float deltaTime = static_cast<float>(currentTime - frameStartTime) / frequency;
		frameStartTime = currentTime;

		frameCount++;

		frameStart = SDL_GetTicks();

		ProcessInput();
		#ifdef EDITOR_BUILD
			UpdateEditor();
		#endif // EDITOR_BUILD

		Update(deltaTime);
		Render();
		
		elapsedTime = SDL_GetTicks() - frameStart;
		
		if (elapsedTime < FRAME_DELAY) {
			SDL_Delay(FRAME_DELAY - elapsedTime);
		}

		if (SDL_GetTicks() - fpsTimer >= 1000) {
			std::string title = "FPS: " + std::to_string(frameCount);
			frameCount = 0;
			fpsTimer += 1000;
		}
	}
}

void Game::Shutdown()
{
	#ifdef EDITOR_BUILD
		m_imguiLayer.Shutdown();
	#endif // EDITOR_BUILD

	m_soundManager.Shutdown();

	m_assetManager.ClearTextures();

	// Cleanup SDL
	if (m_renderer) {
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}

	if (m_window) {
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

	// Cleanup asset manager, physics system and font manager

	m_physicsSystem.Shutdown();
	m_fontManager.Shutdown();

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
