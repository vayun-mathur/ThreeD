#include "AudioSound.h"
#include <al.h>
#include <alc.h>
#include <string>
#include <iostream>
#include <fstream>
#include "alhelper.h"

int n = 1;
#define little_endian *(char *)&n == 1

std::int32_t convert_to_int(char* buffer, std::size_t len)
{
	std::int32_t a = 0;
	if (little_endian)
		std::memcpy(&a, buffer, len);
	else
		for (std::size_t i = 0; i < len; ++i)
			reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
	return a;
}

bool load_wav_file_header(std::ifstream& file,
	std::uint8_t& channels,
	std::int32_t& sampleRate,
	std::uint8_t& bitsPerSample,
	ALsizei& size)
{
	char buffer[4];
	if (!file.is_open())
		return false;

	// the RIFF
	if (!file.read(buffer, 4))
	{
		std::cerr << "ERROR: could not read RIFF" << std::endl;
		return false;
	}
	if (std::strncmp(buffer, "RIFF", 4) != 0)
	{
		std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
		return false;
	}

	// the size of the file
	if (!file.read(buffer, 4))
	{
		std::cerr << "ERROR: could not read size of file" << std::endl;
		return false;
	}

	// the WAVE
	if (!file.read(buffer, 4))
	{
		std::cerr << "ERROR: could not read WAVE" << std::endl;
		return false;
	}
	if (std::strncmp(buffer, "WAVE", 4) != 0)
	{
		std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
		return false;
	}

	// "fmt/0"
	if (!file.read(buffer, 4))
	{
		std::cerr << "ERROR: could not read fmt/0" << std::endl;
		return false;
	}

	// this is always 16, the size of the fmt data chunk
	if (!file.read(buffer, 4))
	{
		std::cerr << "ERROR: could not read the 16" << std::endl;
		return false;
	}

	// PCM should be 1?
	if (!file.read(buffer, 2))
	{
		std::cerr << "ERROR: could not read PCM" << std::endl;
		return false;
	}

	// the number of channels
	if (!file.read(buffer, 2))
	{
		std::cerr << "ERROR: could not read number of channels" << std::endl;
		return false;
	}
	channels = convert_to_int(buffer, 2);

	// sample rate
	if (!file.read(buffer, 4))
	{
		std::cerr << "ERROR: could not read sample rate" << std::endl;
		return false;
	}
	sampleRate = convert_to_int(buffer, 4);

	// (sampleRate * bitsPerSample * channels) / 8
	if (!file.read(buffer, 4))
	{
		std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
		return false;
	}

	// ?? dafaq
	if (!file.read(buffer, 2))
	{
		std::cerr << "ERROR: could not read dafaq" << std::endl;
		return false;
	}

	// bitsPerSample
	if (!file.read(buffer, 2))
	{
		std::cerr << "ERROR: could not read bits per sample" << std::endl;
		return false;
	}
	bitsPerSample = convert_to_int(buffer, 2);

	// data chunk header "data"
	if (!file.read(buffer, 4))
	{
		std::cerr << "ERROR: could not read data chunk header" << std::endl;
		return false;
	}
	if (std::strncmp(buffer, "data", 4) != 0)
	{
		std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
		return false;
	}

	// size of data
	if (!file.read(buffer, 4))
	{
		std::cerr << "ERROR: could not read data size" << std::endl;
		return false;
	}
	size = convert_to_int(buffer, 4);

	/* cannot be at the end of file */
	if (file.eof())
	{
		std::cerr << "ERROR: reached EOF on the file" << std::endl;
		return false;
	}
	if (file.fail())
	{
		std::cerr << "ERROR: fail state set on the file" << std::endl;
		return false;
	}

	return true;
}

char* load_wav(const std::string& filename,
	std::uint8_t& channels,
	std::int32_t& sampleRate,
	std::uint8_t& bitsPerSample,
	ALsizei& size)
{
	std::ifstream in(filename, std::ios::binary);
	if (!in.is_open())
	{
		std::cerr << "ERROR: Could not open \"" << filename << "\"" << std::endl;
		return nullptr;
	}
	if (!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
	{
		std::cerr << "ERROR: Could not load wav header of \"" << filename << "\"" << std::endl;
		return nullptr;
	}

	char* data = new char[size];

	in.read(data, size);

	return data;
}

AudioSound::AudioSound(const wchar_t* full_path) : Resource(full_path)
{
	std::uint8_t channels;
	std::int32_t sampleRate;
	std::uint8_t bitsPerSample;
	ALsizei size;
	std::wstring ws = std::wstring(full_path);
	char* soundData = load_wav(std::string(ws.begin(),ws.end()), channels, sampleRate, bitsPerSample, size);
	if (!soundData)
	{
		throw std::exception("WAV file not loaded successfully");
	}

	alCall(alGenBuffers, 1, &buffer);

	ALenum format;
	if (channels == 1 && bitsPerSample == 8)
		format = AL_FORMAT_MONO8;
	else if (channels == 1 && bitsPerSample == 16)
		format = AL_FORMAT_MONO16;
	else if (channels == 2 && bitsPerSample == 8)
		format = AL_FORMAT_STEREO8;
	else if (channels == 2 && bitsPerSample == 16)
		format = AL_FORMAT_STEREO16;
	else
	{
		std::cerr
			<< "ERROR: unrecognised wave format: "
			<< channels << " channels, "
			<< bitsPerSample << " bps" << std::endl;
	}

	alCall(alBufferData, buffer, format, soundData, size, sampleRate);
	delete[] soundData;
}

AudioSound::~AudioSound()
{
	alCall(alDeleteBuffers, 1, &buffer);
}
