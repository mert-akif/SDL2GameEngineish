#pragma once

#include <SDL2/SDL_mixer.h>

#include <string>
#include <unordered_map>
#include <iostream>

struct SoundEffectAsset
{
    int id = -1;
    std::string name = "";
    std::string path = "";
    Mix_Chunk* chunk = nullptr;
};

struct MusicAsset
{
    int id = -1;
    std::string name = "";
    std::string path = "";
    Mix_Music* music = nullptr;
};

class SoundManager
{
public:
    bool Init();
    void Shutdown();

    int LoadSoundEffect(
        const std::string& name,
        const std::string& path
    );

    int LoadSoundEffectWithId(
        int id,
        const std::string& name,
        const std::string& path
    );

    int LoadMusic(
        const std::string& name,
        const std::string& path
    );

    int LoadMusicWithId(
        int id,
        const std::string& name,
        const std::string& path
    );

    void PlaySoundEffect(int id, int loops = 0);
    void PlayMusic(int id, int loops = -1);

    void StopMusic();
    void PauseMusic();
    void ResumeMusic();

    void SetSoundVolume(int id, int volume);
    void SetMasterSoundVolume(int volume);
    void SetMusicVolume(int volume);

    bool HasSoundEffect(int id) const;
    bool HasMusic(int id) const;

    const SoundEffectAsset* GetSoundEffect(int id) const;
    const MusicAsset* GetMusic(int id) const;

    const std::unordered_map<int, SoundEffectAsset>& GetSoundEffects() const
    {
        return m_soundEffects;
    }

    const std::unordered_map<int, MusicAsset>& GetMusicAssets() const
    {
        return m_musicAssets;
    }

    void RemoveSoundEffect(int id);
    void RemoveMusic(int id);

    void Clear();

private:
    int m_nextSoundEffectId = 0;
    int m_nextMusicId = 0;

    std::unordered_map<int, SoundEffectAsset> m_soundEffects;
    std::unordered_map<int, MusicAsset> m_musicAssets;
};