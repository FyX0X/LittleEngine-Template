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
		void Update(float dt);
		void Render();
		void Shutdown();

		// must always be set correctly
		void OnWindowSizeChange(int w, int h);


	private:
		
		GameData m_data;
		std::unique_ptr<LittleEngine::Renderer> m_renderer;

		// temporary
		LittleEngine::Texture texture1, texture2;
		LittleEngine::Texture minecraft_blocks;
		LittleEngine::TextureAtlas minecraft_atlas;
		//std::vector<LittleEngine::Texture> textures;
		std::vector<glm::vec4> rect;
		std::vector<glm::vec4> rect_uv;
		LittleEngine::Color color = LittleEngine::Colors::White;
		LittleEngine::Font font;

		float scale = 1.f;

		float delta = 0;

		float speed = 1.f;

		static const unsigned int world[];

		LittleEngine::TilemapRenderer tilemap;
		std::vector<LittleEngine::AtlasCoord> tileIDs;

		int length = 1;
		bool w = false;
		bool v = true;
		// fps:
		std::vector<float> fpsHistory;
		const int historySize = 1000;
	};





}