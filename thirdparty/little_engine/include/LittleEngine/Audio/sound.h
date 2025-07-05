#pragma once

#include <iostream>;
#include <miniaudio.h>;


namespace LittleEngine::Audio
{

	class Sound
	{
	public:

		Sound() {};
		
		void LoadFromFile(const std::string& path, ma_engine& engine);
		void Shutdown();

		void Play();

		void SetVolume(float volume);
		void SetPitch(float pitch);

	private:

		ma_sound m_sound = {};


	};

}