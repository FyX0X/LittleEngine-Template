#pragma once

#include <iostream>;
#include <miniaudio.h>;


namespace LittleEngine::Audio
{

	class Sound
	{
	public:

		Sound() = default;
		~Sound() { Shutdown(); }

		// prevent copying cause it breaks ma_sound.
		Sound(const Sound&) = delete;
		Sound& operator=(const Sound&) = delete;
		Sound(Sound&&) = delete;
		Sound& operator=(Sound&&) = delete;

		
		void LoadFromFile(const std::string& path, ma_engine& engine);
		void Shutdown();

		void Play();

		void SetVolume(float volume);
		void SetPitch(float pitch);

	private:

		ma_sound m_sound = {};


	};

}