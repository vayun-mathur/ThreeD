#include "AudioSystem.h"
#include <al.h>
#include <alc.h>
#include <iostream>
#include <fstream>
#include <bit>
#include <vector>
#include "AudioSoundManager.h"
#include "AudioSound.h"

#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

bool check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device)
{
	ALCenum error = alcGetError(device);
	if (error != ALC_NO_ERROR)
	{
		std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n";
		switch (error)
		{
		case ALC_INVALID_VALUE:
			std::cerr << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
			break;
		case ALC_INVALID_DEVICE:
			std::cerr << "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
			break;
		case ALC_INVALID_CONTEXT:
			std::cerr << "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
			break;
		case ALC_INVALID_ENUM:
			std::cerr << "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
			break;
		case ALC_OUT_OF_MEMORY:
			std::cerr << "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
			break;
		default:
			std::cerr << "UNKNOWN ALC ERROR: " << error;
		}
		std::cerr << std::endl;
		return false;
	}
	return true;
}

template<typename alcFunction, typename... Params>
auto alcCallImpl(const char* filename,
	const std::uint_fast32_t line,
	alcFunction function,
	ALCdevice* device,
	Params... params)
	->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
	function(std::forward<Params>(params)...);
	return check_alc_errors(filename, line, device);
}

template<typename alcFunction, typename ReturnType, typename... Params>
auto alcCallImpl(const char* filename,
	const std::uint_fast32_t line,
	alcFunction function,
	ReturnType& returnValue,
	ALCdevice* device,
	Params... params)
	->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>
{
	returnValue = function(std::forward<Params>(params)...);
	return check_alc_errors(filename, line, device);
}

AudioSystem::AudioSystem()
{
	openALDevice = alcOpenDevice(nullptr);
	if (!openALDevice)
		throw std::exception("Audio Device not created successfully");

	if (!alcCall(alcCreateContext, openALContext, openALDevice, openALDevice, nullptr) || !openALContext)
	{
		throw std::exception("Audio Context not created successfully");
	}
	contextMadeCurrent = false;
	if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, openALContext)
		|| contextMadeCurrent != ALC_TRUE)
	{
		throw std::exception("Audio Context could not be made current");
	}

	try {
		m_audiosound_manager = new AudioSoundManager();
	}
	catch (...) { throw std::exception("AudioSoundManager not created successfully"); }

	AudioSoundPtr audio = m_audiosound_manager->createAudioSoundFromFile(L"Assets/Audio/CantinaBand60.wav");

	audio->play();
}

AudioSystem::~AudioSystem()
{

	alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, nullptr);
	alcCall(alcDestroyContext, openALDevice, openALContext);

	ALCboolean closed;
	alcCall(alcCloseDevice, closed, openALDevice, openALDevice);
}

AudioSystem* AudioSystem::get()
{
	return m_system;
}

void AudioSystem::create()
{
	if (AudioSystem::m_system) throw std::exception("Audio System already created");
	AudioSystem::m_system = new AudioSystem();
}

void AudioSystem::release()
{
	if (!AudioSystem::m_system) return;
	delete AudioSystem::m_system;
	AudioSystem::m_system = nullptr;
}

AudioSystem* AudioSystem::m_system = nullptr;