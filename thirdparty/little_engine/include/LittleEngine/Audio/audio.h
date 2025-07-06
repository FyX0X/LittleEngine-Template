#pragma once

#include <miniaudio.h>
#include "LittleEngine/Audio/sound.h"


namespace LittleEngine::Audio
{

	class AudioSystem
	{
	public:
		AudioSystem() = default;
		~AudioSystem() { Shutdown(); }

		// prevent copy because ma_engine is not movable (internal state pointers)
		AudioSystem(const AudioSystem&) = delete;
		AudioSystem& operator=(const AudioSystem&) = delete;
		AudioSystem(AudioSystem&&) = delete;
		AudioSystem& operator=(AudioSystem&&) = delete;
		
		void Initialize();
		void Shutdown();

		ma_engine* GetEngine();
		bool IsInitialized() const { return m_initialized; }

		void LoadSound(const std::string& path, Sound& sound);


	private:
		ma_engine m_engine;
		bool m_initialized = false;


	};



}