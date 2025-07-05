#include "LittleEngine/Audio/sound.h"
#include "LittleEngine/error_logger.h"



namespace LittleEngine::Audio
{

	void Sound::LoadFromFile(const std::string& path, ma_engine& engine)
	{
		ma_result result;

		result = ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, &m_sound);
		if (result != MA_SUCCESS) {
			LogError("Sound::Initialize: could not load sound from file: " + path + " (Error Code: " + std::to_string(result) + ')');
			return;
		}
	}

	void Sound::Shutdown()
	{
		ma_sound_uninit(&m_sound);
	}

	void Sound::Play()
	{
		ma_result result;
		result = ma_sound_seek_to_pcm_frame(&m_sound, 0); // Rewind
		if (result != MA_SUCCESS) {
			LogError("Sound::Play: Could not rewind the sound.  (Error Code: " + std::to_string(result) + ')');
			return;
		}
		result = ma_sound_start(&m_sound);
		if (result != MA_SUCCESS) {
			LogError("Sound::Sound(): could not play the sound.  (Error Code: " + std::to_string(result) + ')');
			return;
		}

	}

	void Sound::SetVolume(float volume)
	{
		ma_sound_set_volume(&m_sound, volume);
	}

	void Sound::SetPitch(float pitch)
	{
		ma_sound_set_pitch(&m_sound, pitch);
	}
}

