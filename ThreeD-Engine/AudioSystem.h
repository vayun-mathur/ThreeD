#pragma once
#include "prerequisites.h"

class AudioSystem
{
public:
	AudioSystem();
	~AudioSystem();
public:
	static AudioSystem* get();
	static void create();
	static void release();
private:
	static AudioSystem* m_system;
};