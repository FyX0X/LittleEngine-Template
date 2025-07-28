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
		
		//struct LightSource
		//{
		//	glm::vec2 position = { 0.f, 0.f };
		//	glm::vec3 color = { 1.f, 1.f, 1.f };
		//	float intensity = 1.f;
		//	float radius = 1.f;
		//};

		//struct Edge
		//{
		//	glm::vec2 p1;
		//	glm::vec2 p2;
		//	float length() const { return glm::length(p2 - p1); }
		//	glm::vec2 direction() const { return glm::normalize(p2 - p1); }
		//};

		//struct Polygon
		//{
		//	std::vector<glm::vec2> vertices;
		//	std::vector<Edge> edges;

		//	void BuildEdges()
		//	{
		//		edges.clear();
		//		for (size_t i = 0; i < vertices.size(); ++i)
		//		{
		//			glm::vec2 p1 = vertices[i];
		//			glm::vec2 p2 = vertices[(i + 1) % vertices.size()];
		//			edges.push_back({ p1, p2 });
		//		}
		//	}
		//};

		//bool edgeFacesLight(const Edge& edge, const glm::vec2& lightPos)
		//{
		//	glm::vec2 edgeDir = edge.p2 - edge.p1;
		//	glm::vec2 edgeNormal = glm::normalize(glm::vec2(edgeDir.y, -edgeDir.x)); // right normal in CCW order is outside

		//	glm::vec2 lightToEdge = glm::normalize(edge.p1 - lightPos);

		//	
		//	return glm::dot(edgeNormal, lightToEdge) < 0.f;
		//}

		//struct ShadowQuad
		//{
		//	glm::vec2 p1, p2, p3, p4; // vertices of the shadow quad
		//};

		//std::vector<ShadowQuad> CalculateShadowQuads(const LightSource& light, const Polygon& poly)
		//{
		//	std::vector<ShadowQuad> shadowQuads;
		//	for (const Edge& edge : poly.edges)
		//	{
		//		if (edgeFacesLight(edge, light.position))
		//		{

		//			glm::vec2 dir1 = glm::normalize(edge.p1 - light.position);
		//			glm::vec2 dir2 = glm::normalize(edge.p2 - light.position);
		//			
		//			// Create shadow quad vertices
		//			glm::vec2 p1 = edge.p1;
		//			glm::vec2 p2 = edge.p2;
		//			glm::vec2 p3 = edge.p2 + dir2 * light.radius * 100.f;
		//			glm::vec2 p4 = edge.p1 + dir1 * light.radius * 100.f;
		//			shadowQuads.push_back({ p1, p2, p3, p4 });
		//		}
		//	}
		//	return shadowQuads;
		//}

		//std::vector<glm::vec2> GetShadowTriangles(const std::vector<ShadowQuad>& shadowQuads)
		//{
		//	std::vector<glm::vec2> triangles;
		//	for (const ShadowQuad& quad : shadowQuads)
		//	{
		//		// Create two triangles from the quad
		//		triangles.push_back(quad.p1);
		//		triangles.push_back(quad.p2);
		//		triangles.push_back(quad.p3);
		//		triangles.push_back(quad.p1);
		//		triangles.push_back(quad.p3);
		//		triangles.push_back(quad.p4);
		//	}
		//	return triangles;
		//}


		//std::vector<LightSource> sources = { 
		//	{ { 3.f, 2.f }, { 1.f, 1.f, 1.f }, 1.f, 5.f},
		//	{ { -3.f, -2.f }, { 1.f, 0.5f, 0.5f }, 1.f, 10.f },
		//	{ { 0.f, 0.f }, { 0.5f, 0.5f, 1.f }, 3.f, 15.f}
		//};


		//std::vector<Polygon> obstacles = {};

		GLuint shadowVAO = 0;
		GLuint shadowVBO = 0;

		void InitializeShadowBuffers()
		{
			if (shadowVAO == 0)
			{
				glGenVertexArrays(1, &shadowVAO);
				glGenBuffers(1, &shadowVBO);
				glBindVertexArray(shadowVAO);
				glBindBuffer(GL_ARRAY_BUFFER, shadowVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, nullptr, GL_DYNAMIC_DRAW);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
				glEnableVertexAttribArray(0);
				glBindVertexArray(0);
			}
		}


		GameData m_data;
		std::unique_ptr<LittleEngine::Graphics::Renderer> m_renderer;
		std::unique_ptr<LittleEngine::Audio::AudioSystem> m_audioSystem;

		// temporary

		bool outlineMode = false;
		LittleEngine::Polygon polygon = {};

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
		LittleEngine::Graphics::RenderTarget tempLightFBO = {};

		LittleEngine::Graphics::Camera sceneCamera = {};
		LittleEngine::Graphics::Camera UICamera = {};


		LittleEngine::Graphics::Shader blurShader = {};
		LittleEngine::Graphics::Shader lightSceneMergingShader = {};

		LittleEngine::Graphics::Shader lightShader = {};
		LittleEngine::Graphics::Shader shadowShader = {};


		LittleEngine::Graphics::Shader fullscreenShader = {};
		LittleEngine::Graphics::Shader fullscreenImageBlitShader = {};

		bool enableShadows = true;


	};





}