#pragma once
#include <LittleEngine/little_engine.h>

#include "gameData.h"


namespace game
{

	class Game
	{

	public:
		Game() {};
		~Game() = default;

		Game(Game& other) = delete;
		Game(Game&& other) = delete;
		Game operator=(Game other) = delete;
		Game operator=(Game& other) = delete;
		Game operator=(Game&& other) = delete;

		bool Initialize();
		void Shutdown();

		void Update(float dt);
		void Render();

		// must always be set correctly
		void OnWindowSizeChange(int w, int h);


	private:


		void ResizeFBOs();

		void BlurLightTexture(LittleEngine::Graphics::RenderTarget& lightFBO, int passes, LittleEngine::Graphics::Shader& shader);
		
		struct LightSource
		{
			glm::vec2 position;
			glm::vec3 color = { 1.f, 1.f, 1.f };
			float intensity = 1.f;
			float radius = 1.f;
		};

		struct edge
		{
			glm::vec2 p1;
			glm::vec2 p2;
			float length() const { return glm::length(p2 - p1); }
			glm::vec2 direction() const { return glm::normalize(p2 - p1); }
		};

		LightSource light = { { 3.f, 2.f }, { 1.f, 1.f, 1.f }, 1.f, 1.f };
		std::vector<edge> edges = {
			{{-1.f, -1.f}, {1.f, -1.f}},
			{{1.f, -1.f}, {1.f, 1.f}},
			{{1.f, 1.f}, {-1.f, 1.f}},
			{{-1.f, 1.f}, {-1.f, -1.f}}
		};


		GameData m_data;
		std::unique_ptr<LittleEngine::Graphics::Renderer> m_renderer;
		std::unique_ptr<LittleEngine::Audio::AudioSystem> m_audioSystem;

		// temporary
		LittleEngine::Audio::Sound sound;
		float pitch = 1.f;
		float volume = 1.f;
		bool spatialized = false;
		float minD = .5f;
		float maxD = 10.f;
		float maxG = 1.f;
		float minG = .1f;
		float rolloff = 1.f;

		LittleEngine::Graphics::Texture texture1, texture2;
		LittleEngine::Graphics::Texture torch;
		LittleEngine::Graphics::Texture minecraft_blocks;
		LittleEngine::Graphics::TextureAtlas minecraft_atlas;
		std::vector<LittleEngine::Graphics::Texture> textures;
		std::vector<glm::vec4> rect;
		std::vector<glm::vec4> rect_uv;
		LittleEngine::Graphics::Color color = LittleEngine::Graphics::Colors::White;
		LittleEngine::Graphics::Font font;

		float scale = 1.f;

		float delta = 0;

		float speed = 10.f;


		int blurPasses = 5;
		int downscaleFactor = 2;
		float lightIntensity = 1.f;

		float cameraFollowSpeed = 30.f;
		float maxDist = 5.f;
		float minDist = 0.f;

		static const unsigned int world[];

		LittleEngine::Graphics::TilemapRenderer tilemap;
		std::vector<LittleEngine::Graphics::AtlasCoord> tileIDs;

		int length = 1;
		bool w = false;
		bool v = true;
		// fps:
		std::vector<float> fpsHistory;
		std::vector<float> distHistory;
		const int historySize = 1000;


		LittleEngine::Graphics::RenderTarget target = {};
		LittleEngine::Graphics::RenderTarget sceneFBO = {};
		LittleEngine::Graphics::RenderTarget lightFBO = {};

		LittleEngine::Graphics::Camera sceneCamera = {};
		LittleEngine::Graphics::Camera UICamera = {};


		LittleEngine::Graphics::Shader blurShader = {};
		LittleEngine::Graphics::Shader lightSceneMergingShader = {};

	};





}