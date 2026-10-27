#include "SceneManager.h"

static std::string GetValue(const std::string& line)
{
    size_t pos = line.find('=');

    if (pos == std::string::npos)
        return "";

    return line.substr(pos + 1);
}

bool SceneSerializer::SaveScene(
    const std::string& path,
    const std::vector<GameObject>& gameObjects,
    const AssetManager& assetManager,
    const AnimationManager& animationManager,
    const SoundManager& soundManager
)
{
    std::ofstream file(path);

    if (!file.is_open())
    {
        std::cout << "Scene save failed: " << path << std::endl;
        return false;
    }

    // Texture Library
    for (const auto& pair : assetManager.GetTextures())
    {
        const std::string& key = pair.first;
        const TextureAsset& asset = pair.second;

        if (key.empty() || asset.path.empty())
            continue;

        file << "TextureAsset\n";
        file << "key=" << key << "\n";
        file << "path=" << asset.path << "\n";
        file << "EndTextureAsset\n";
    }


    for (const AnimationClip& clip : animationManager.GetClips())
    {
        file << "AnimationClip\n";
        file << "id=" << clip.id << "\n";
        file << "name=" << clip.name << "\n";
		file << "textureKey=" << clip.textureKey << "\n";
		file << "texturePath=" << clip.texturePath << "\n";
        file << "frameCount=" << clip.frameCount << "\n";
        file << "frameWidth=" << clip.frameWidth << "\n";
        file << "frameHeight=" << clip.frameHeight << "\n";
        file << "fps=" << clip.fps << "\n";
        file << "EndAnimationClip\n";
    }

    // Sound Library
    for (const auto& pair : soundManager.GetSoundEffects())
    {
        const SoundEffectAsset& sound = pair.second;

        if (sound.path.empty())
            continue;

        file << "SoundEffect\n";
        file << "id=" << sound.id << "\n";
        file << "name=" << sound.name << "\n";
        file << "path=" << sound.path << "\n";
        file << "EndSoundEffect\n";
    }

    for (const auto& pair : soundManager.GetMusicAssets())
    {
        const MusicAsset& music = pair.second;

        if (music.path.empty())
            continue;

        file << "Music\n";
        file << "id=" << music.id << "\n";
        file << "name=" << music.name << "\n";
        file << "path=" << music.path << "\n";
        file << "EndMusic\n";
    }

    // GameObjects
    for (const GameObject& object : gameObjects)
    {
        file << "GameObject\n";

        file << "id=" << object.id << "\n";
        file << "name=" << object.name << "\n";

        file << "x=" << object.x << "\n";
        file << "y=" << object.y << "\n";
        file << "width=" << object.width << "\n";
        file << "height=" << object.height << "\n";
        file << "orderInLayer=" << object.orderInLayer << "\n";
        file << "active=" << object.active << "\n";

        file << "textureKey=" << object.textureKey << "\n";

        file << "hasAnimation=" << object.hasAnimation << "\n";
        file << "animationId=" << object.animationId << "\n";

        file << "usePhysics=" << object.usePhysics << "\n";
        file << "isStatic=" << object.isStatic << "\n";
        file << "mass=" << object.mass << "\n";
        file << "gravityScale=" << object.gravityScale << "\n";
        file << "velocityX=" << object.velocityX << "\n";
        file << "velocityY=" << object.velocityY << "\n";

        file << "hasCollider=" << object.hasCollider << "\n";
        file << "isTrigger=" << object.isTrigger << "\n";

        file << "hasScript=" << object.hasScript << "\n";
        file << "scriptName=" << object.scriptName << "\n";

        file << "End\n";
    }

    file.close();

    std::cout << "Scene saved: " << path << std::endl;
    return true;
}
bool SceneSerializer::LoadScene(
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
)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cout << "Scene load failed: " << path << std::endl;
        return false;
    }

    gameObjects.clear();
    nextGameObjectId = 0;

    std::string line;

    GameObject object;
    bool readingObject = false;

    std::string tempTextureKey;
    std::string tempTexturePath;
    bool readingTextureAsset = false;

    AnimationClip tempClip;
    bool readingAnimationClip = false;

    SoundEffectAsset tempSound;
    bool readingSoundEffect = false;

    MusicAsset tempMusic;
    bool readingMusic = false;

    while (std::getline(file, line))
    {
        // -------------------------
        // TextureAsset block start/end
        // -------------------------
        if (line == "TextureAsset")
        {
            tempTextureKey = "";
            tempTexturePath = "";
            readingTextureAsset = true;
            continue;
        }

        if (line == "EndTextureAsset")
        {
            if (!tempTextureKey.empty() && !tempTexturePath.empty())
            {
                assetManager.LoadTexture(tempTextureKey, tempTexturePath);
            }
            else
            {
                std::cout << "Skipped texture with empty key/path." << std::endl;
            }

            readingTextureAsset = false;
            continue;
        }

        if (readingTextureAsset)
        {
            if (line.rfind("key=", 0) == 0)
                tempTextureKey = GetValue(line);

            else if (line.rfind("path=", 0) == 0)
                tempTexturePath = GetValue(line);

            continue;
        }

        // -------------------------
        // AnimationClip block start/end
        // -------------------------
        if (line == "AnimationClip")
        {
            tempClip = AnimationClip();
            readingAnimationClip = true;
            continue;
        }

        if (line == "EndAnimationClip")
        {
            if (!tempClip.texturePath.empty() && tempClip.frameCount > 0)
            {
                std::string textureKey = tempClip.textureKey;

                if (textureKey.empty())
                {
                    textureKey = "anim_clip_" + std::to_string(tempClip.id);
                }

                SDL_Texture* animTexture = assetManager.LoadTexture(
                    textureKey,
                    tempClip.texturePath
                );

                if (animTexture)
                {
                    animationManager.CreateClip(
                        animTexture,
                        textureKey,
                        tempClip.texturePath,
                        tempClip.frameCount,
                        tempClip.id,
                        tempClip.frameWidth,
                        tempClip.frameHeight,
                        tempClip.fps
                    );
                }
                else
                {
                    std::cout << "Animation texture load failed. Clip ID: "
                        << tempClip.id << std::endl;
                }
            }
            else
            {
                std::cout << "Skipped animation clip. ID: "
                    << tempClip.id
                    << " path/frameCount invalid."
                    << std::endl;
            }

            readingAnimationClip = false;
            continue;
        }

        if (readingAnimationClip)
        {
            if (line.rfind("id=", 0) == 0)
                tempClip.id = std::stoi(GetValue(line));

            else if (line.rfind("name=", 0) == 0)
                tempClip.name = GetValue(line);

            else if (line.rfind("textureKey=", 0) == 0)
                tempClip.textureKey = GetValue(line);

            else if (line.rfind("texturePath=", 0) == 0)
                tempClip.texturePath = GetValue(line);

            // Eski scene dosyan için destek:
            else if (line.rfind("path=", 0) == 0)
                tempClip.texturePath = GetValue(line);

            else if (line.rfind("frameCount=", 0) == 0)
                tempClip.frameCount = std::stoi(GetValue(line));

            else if (line.rfind("frameWidth=", 0) == 0)
                tempClip.frameWidth = std::stoi(GetValue(line));

            else if (line.rfind("frameHeight=", 0) == 0)
                tempClip.frameHeight = std::stoi(GetValue(line));

            else if (line.rfind("fps=", 0) == 0)
                tempClip.fps = std::stof(GetValue(line));

            continue;
        }

        // -------------------------
        // SoundEffect block start/end
        // -------------------------
        if (line == "SoundEffect")
        {
            tempSound = SoundEffectAsset();
            readingSoundEffect = true;
            continue;
        }

        if (line == "EndSoundEffect")
        {
            if (!tempSound.path.empty())
            {
                soundManager.LoadSoundEffectWithId(
                    tempSound.id,
                    tempSound.name,
                    tempSound.path
                );
            }
            else
            {
                std::cout << "Skipped sound effect with empty path. ID: "
                    << tempSound.id << std::endl;
            }

            readingSoundEffect = false;
            continue;
        }

        if (readingSoundEffect)
        {
            if (line.rfind("id=", 0) == 0)
                tempSound.id = std::stoi(GetValue(line));

            else if (line.rfind("name=", 0) == 0)
                tempSound.name = GetValue(line);

            else if (line.rfind("path=", 0) == 0)
                tempSound.path = GetValue(line);

            continue;
        }

        // -------------------------
        // Music block start/end
        // -------------------------
        if (line == "Music")
        {
            tempMusic = MusicAsset();
            readingMusic = true;
            continue;
        }

        if (line == "EndMusic")
        {
            if (!tempMusic.path.empty())
            {
                soundManager.LoadMusicWithId(
                    tempMusic.id,
                    tempMusic.name,
                    tempMusic.path
                );
            }
            else
            {
                std::cout << "Skipped music with empty path. ID: "
                    << tempMusic.id << std::endl;
            }

            readingMusic = false;
            continue;
        }

        if (readingMusic)
        {
            if (line.rfind("id=", 0) == 0)
                tempMusic.id = std::stoi(GetValue(line));

            else if (line.rfind("name=", 0) == 0)
                tempMusic.name = GetValue(line);

            else if (line.rfind("path=", 0) == 0)
                tempMusic.path = GetValue(line);

            continue;
        }

        // -------------------------
        // GameObject block start/end
        // -------------------------
        if (line == "GameObject")
        {
            object = GameObject();
            readingObject = true;
            continue;
        }

        if (line == "End")
        {
            if (readingObject)
            {
                SDL_Texture* texture = nullptr;

                if (!object.textureKey.empty())
                {
                    texture = assetManager.GetTexture(object.textureKey);

                    if (!texture)
                    {
                        std::cout << "Texture not found in AssetManager. Key: "
                            << object.textureKey << std::endl;
                    }
                }

                object.Init(
                    physicsSystem,
                    texture,
                    &animationManager,
                    ppm
                );

                object.texture = texture;
                object.animationManager = &animationManager;

                object.RefreshPosition();
                object.RefreshSize();
                object.RefreshBodyType();
                object.RefreshVelocity();

                if (object.hasAnimation)
                {
                    object.RefreshAnimationSystem();
                }

                nextGameObjectId = std::max(nextGameObjectId, object.id + 1);

                gameObjects.push_back(std::move(object));
            }

            readingObject = false;
            continue;
        }

        if (!readingObject)
            continue;

        // -------------------------
        // GameObject properties
        // -------------------------
        if (line.rfind("id=", 0) == 0)
            object.id = std::stoi(GetValue(line));

        else if (line.rfind("name=", 0) == 0)
            object.name = GetValue(line);

        else if (line.rfind("x=", 0) == 0)
            object.x = std::stof(GetValue(line));

        else if (line.rfind("y=", 0) == 0)
            object.y = std::stof(GetValue(line));

        else if (line.rfind("width=", 0) == 0)
            object.width = std::stof(GetValue(line));

        else if (line.rfind("height=", 0) == 0)
            object.height = std::stof(GetValue(line));

        else if (line.rfind("orderInLayer=", 0) == 0)
            object.orderInLayer = std::stoi(GetValue(line));

        else if (line.rfind("active=", 0) == 0)
            object.active = std::stoi(GetValue(line)) != 0;

        else if (line.rfind("textureKey=", 0) == 0)
            object.textureKey = GetValue(line);

        else if (line.rfind("hasAnimation=", 0) == 0)
            object.hasAnimation = std::stoi(GetValue(line)) != 0;

        else if (line.rfind("animationId=", 0) == 0)
            object.animationId = std::stoi(GetValue(line));

        else if (line.rfind("usePhysics=", 0) == 0)
            object.usePhysics = std::stoi(GetValue(line)) != 0;

        else if (line.rfind("isStatic=", 0) == 0)
            object.isStatic = std::stoi(GetValue(line)) != 0;

        else if (line.rfind("mass=", 0) == 0)
            object.mass = std::stof(GetValue(line));

        else if (line.rfind("gravityScale=", 0) == 0)
            object.gravityScale = std::stof(GetValue(line));

        else if (line.rfind("velocityX=", 0) == 0)
            object.velocityX = std::stof(GetValue(line));

        else if (line.rfind("velocityY=", 0) == 0)
            object.velocityY = std::stof(GetValue(line));

        else if (line.rfind("hasCollider=", 0) == 0)
            object.hasCollider = std::stoi(GetValue(line)) != 0;

        else if (line.rfind("isTrigger=", 0) == 0)
            object.isTrigger = std::stoi(GetValue(line)) != 0;

        else if (line.rfind("hasScript=", 0) == 0)
            object.hasScript = std::stoi(GetValue(line)) != 0;

        else if (line.rfind("scriptName=", 0) == 0)
            object.scriptName = GetValue(line);
    }

    file.close();

    // Scriptleri en sonda takıyoruz.
    // Çünkü OnCreate içinde animation/sound çağrılabilir.
    for (GameObject& obj : gameObjects)
    {
        if (obj.hasScript && !obj.scriptName.empty())
        {
            std::unique_ptr<Script> loadedScript =
                scriptRegistry.CreateScript(obj.scriptName);

            if (loadedScript)
            {
                obj.SetScript(std::move(loadedScript), scriptContext);
            }
            else
            {
                std::cout << "Script not found: "
                    << obj.scriptName << std::endl;

                obj.hasScript = false;
                obj.scriptName = "";
            }
        }
    }

    std::cout << "Scene loaded: " << path << std::endl;
    return true;
}