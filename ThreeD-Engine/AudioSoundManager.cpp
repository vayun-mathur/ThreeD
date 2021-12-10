#include "AudioSoundManager.h"
#include "AudioSound.h"

AudioSoundManager::AudioSoundManager() : ResourceManager()
{
}

AudioSoundManager::~AudioSoundManager()
{
}

AudioSoundPtr AudioSoundManager::createAudioSoundFromFile(const wchar_t* file_path)
{
    return std::static_pointer_cast<AudioSound>(createResourceFromFile(file_path));
}

Resource* AudioSoundManager::createResourceFromFileConcrete(const wchar_t* file_path)
{
    AudioSound* tex = nullptr;
    try {
        tex = new AudioSound(file_path);
    }
    catch (...) {}
    return tex;
}
