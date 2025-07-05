#include "LittleEngine/Audio/audio.h"
#include "LittleEngine/error_logger.h"



namespace LittleEngine::Audio
{
    void AudioSystem::Initialize() {
        ma_engine_config config = ma_engine_config_init();
        ma_result result = ma_engine_init(&config, &m_engine);
        if (result != MA_SUCCESS) {
            LogError("AudioSystem::Initialize failed: " + std::to_string(result));
            m_initialized = false;
            return;
        }
        m_initialized = true;

    }
    void AudioSystem::Shutdown()
    {
        ma_engine_uninit(&m_engine);
    }

    ma_engine* AudioSystem::GetEngine()
    {
        return m_initialized ? &m_engine : nullptr;
    }

    void AudioSystem::LoadSound(const std::string& path, Sound& sound)
    {
        sound.LoadFromFile(path, m_engine);
    }
}