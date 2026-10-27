#include "SoundManager.h"

bool SoundManager::Init()
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cout << "SDL_mixer init failed: " << Mix_GetError() << std::endl;
        return false;
    }

    Mix_AllocateChannels(32);

    std::cout << "SoundManager initialized." << std::endl;
    return true;
}

void SoundManager::Shutdown()
{
    Clear();
    Mix_CloseAudio();

    std::cout << "SoundManager shutdown." << std::endl;
}

int SoundManager::LoadSoundEffect(
    const std::string& name,
    const std::string& path
)
{
    int id = m_nextSoundEffectId++;
    return LoadSoundEffectWithId(id, name, path);
}

int SoundManager::LoadSoundEffectWithId(
    int id,
    const std::string& name,
    const std::string& path
)
{
    if (HasSoundEffect(id))
        return id;

    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());

    if (!chunk)
    {
        std::cout << "Sound effect load failed: " << path << std::endl;
        std::cout << Mix_GetError() << std::endl;
        return -1;
    }

    SoundEffectAsset asset;
    asset.id = id;
    asset.name = name;
    asset.path = path;
    asset.chunk = chunk;

    m_soundEffects[id] = asset;

    if (id >= m_nextSoundEffectId)
        m_nextSoundEffectId = id + 1;

    std::cout << "Sound effect loaded. ID: "
        << id
        << " Name: "
        << name
        << " Path: "
        << path
        << std::endl;

    return id;
}

int SoundManager::LoadMusic(
    const std::string& name,
    const std::string& path
)
{
    int id = m_nextMusicId++;
    return LoadMusicWithId(id, name, path);
}

int SoundManager::LoadMusicWithId(
    int id,
    const std::string& name,
    const std::string& path
)
{
    if (HasMusic(id))
        return id;

    Mix_Music* music = Mix_LoadMUS(path.c_str());

    if (!music)
    {
        std::cout << "Music load failed: " << path << std::endl;
        std::cout << Mix_GetError() << std::endl;
        return -1;
    }

    MusicAsset asset;
    asset.id = id;
    asset.name = name;
    asset.path = path;
    asset.music = music;

    m_musicAssets[id] = asset;

    if (id >= m_nextMusicId)
        m_nextMusicId = id + 1;

    std::cout << "Music loaded. ID: "
        << id
        << " Name: "
        << name
        << " Path: "
        << path
        << std::endl;

    return id;
}

void SoundManager::PlaySoundEffect(int id, int loops)
{
    auto it = m_soundEffects.find(id);

    if (it == m_soundEffects.end())
    {
        std::cout << "Sound effect not found. ID: " << id << std::endl;
        return;
    }

    if (!it->second.chunk)
        return;

    Mix_PlayChannel(-1, it->second.chunk, loops);
}

void SoundManager::PlayMusic(int id, int loops)
{
    auto it = m_musicAssets.find(id);

    if (it == m_musicAssets.end())
    {
        std::cout << "Music not found. ID: " << id << std::endl;
        return;
    }

    if (!it->second.music)
        return;

    Mix_PlayMusic(it->second.music, loops);
}

void SoundManager::StopMusic()
{
    Mix_HaltMusic();
}

void SoundManager::PauseMusic()
{
    Mix_PauseMusic();
}

void SoundManager::ResumeMusic()
{
    Mix_ResumeMusic();
}

void SoundManager::SetSoundVolume(int id, int volume)
{
    auto it = m_soundEffects.find(id);

    if (it == m_soundEffects.end())
        return;

    Mix_VolumeChunk(it->second.chunk, volume);
}

void SoundManager::SetMasterSoundVolume(int volume)
{
    for (auto& pair : m_soundEffects)
    {
        if (pair.second.chunk)
            Mix_VolumeChunk(pair.second.chunk, volume);
    }
}

void SoundManager::SetMusicVolume(int volume)
{
    Mix_VolumeMusic(volume);
}

bool SoundManager::HasSoundEffect(int id) const
{
    return m_soundEffects.find(id) != m_soundEffects.end();
}

bool SoundManager::HasMusic(int id) const
{
    return m_musicAssets.find(id) != m_musicAssets.end();
}

const SoundEffectAsset* SoundManager::GetSoundEffect(int id) const
{
    auto it = m_soundEffects.find(id);

    if (it == m_soundEffects.end())
        return nullptr;

    return &it->second;
}

const MusicAsset* SoundManager::GetMusic(int id) const
{
    auto it = m_musicAssets.find(id);

    if (it == m_musicAssets.end())
        return nullptr;

    return &it->second;
}

void SoundManager::RemoveSoundEffect(int id)
{
    auto it = m_soundEffects.find(id);

    if (it == m_soundEffects.end())
        return;

    if (it->second.chunk)
        Mix_FreeChunk(it->second.chunk);

    m_soundEffects.erase(it);
}

void SoundManager::RemoveMusic(int id)
{
    auto it = m_musicAssets.find(id);

    if (it == m_musicAssets.end())
        return;

    if (it->second.music)
        Mix_FreeMusic(it->second.music);

    m_musicAssets.erase(it);
}

void SoundManager::Clear()
{
    for (auto& pair : m_soundEffects)
    {
        if (pair.second.chunk)
            Mix_FreeChunk(pair.second.chunk);
    }

    m_soundEffects.clear();

    for (auto& pair : m_musicAssets)
    {
        if (pair.second.music)
            Mix_FreeMusic(pair.second.music);
    }

    m_musicAssets.clear();

    m_nextSoundEffectId = 0;
    m_nextMusicId = 0;
}