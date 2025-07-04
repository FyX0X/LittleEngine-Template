#pragma once
#include "LittleEngine/Graphics/camera.h"
#include "LittleEngine/Graphics/shader.h"
#include "LittleEngine/Graphics/color.h"
#include "LittleEngine/Graphics/texture.h"
#include "LittleEngine/Graphics/font.h"
#include <vector>
#include <unordered_map>


namespace LittleEngine
{
	// Represents a rectangle {x, y, w, h} where x, y is the bottom left position.
	using Rect = glm::vec4;


	struct Vertex {
		glm::vec2 pos;
		glm::vec2 uv;
		Color color;
		float textureIndex;
	};


	class Renderer
	{
	public:

		Renderer() {};
		~Renderer() { Shutdown(); }

		Renderer(Renderer& other) = delete;
		Renderer(Renderer&& other) = delete;
		Renderer operator=(Renderer other) = delete;
		Renderer operator=(Renderer& other) = delete;
		Renderer operator=(Renderer&& other) = delete;

		void Initialize(glm::ivec2 size, unsigned int quadCount = defaults::QuadCount);
		void Shutdown();
		void UpdateWindowSize(glm::ivec2 size) 
		{
			UpdateWindowSize(size.x, size.y);
		}
		void UpdateWindowSize(int w, int h);


		void BeginFrame();
		void EndFrame();

#pragma region DRAW RECT

		// Render functions
		void DrawRect(const Rect& rect, const Color& color = Colors::White)
		{
			Color colors[4] = { color, color, color, color };
			DrawRect(rect, s_defaultTexture, colors, {0, 0, 1, 1});
		}
		void DrawRect(const Rect& rect, Texture texture, const Color& color = Colors::White)
		{
			Color colors[4] = { color, color, color, color };
			DrawRect(rect, texture, colors, { 0, 0, 1, 1 });
		}
		void DrawRect(const Rect& rect, Texture texture, const Color& color, const glm::vec4& uv)
		{
			Color colors[4] = { color, color, color, color };
			DrawRect(rect, texture, colors, uv);
		}
		void DrawRect(const Rect& rect, Texture texture, const Color colors[4], const glm::vec4& uv);

#pragma endregion

#pragma region DRAW TEXT

		void DrawString(const std::string& text, const glm::vec2 pos, Color color = Colors::White, float scale = 1.f) 
		{
			DrawString(text, pos, s_defaultFont, color, scale);
		}
		void DrawString(const std::string& text, const glm::vec2 pos, const Font& font, Color color = Colors::White, float scale = 1.f);


#pragma endregion

		int GetQuadCount() { return m_quadCount; }



		Color clearColor = Color( 0.2f, 0.3f, 0.3f, 1.0f );
		Camera camera = {};
		Shader shader;

	private:
		// TODO REFACTOR AUTO FLUSH IF FULL NOT AT END
		void Flush();
		// FLUSH BATCH IF: > 16 textures || #indices > 65536 (or > 10K quads)
		void RenderBatch();
		float AddTextureToBatch(Texture texture);
		void ClearDrawQueue();
		void ClearBatch();
		glm::mat4 GetProjectionMatrix();

		bool m_isInitialized = false;

		static Texture s_defaultTexture;
		static Font s_defaultFont;

		int m_width = -1;
		int m_height = -1;

		int m_quadCount = 0;

		GLuint m_VAO = 0;
		GLuint m_VBO = 0;
		GLuint m_EBO = 0;


		std::vector<Vertex> m_vertices;
		std::vector<Vertex> m_verticesBatch;
		std::vector<unsigned int> m_indices;
		std::vector<unsigned int> m_indicesBatch;
		std::vector<Texture> m_textures;
		std::vector<Texture> m_texturesBatch;
		std::unordered_map<GLuint, int> m_bindedTextures;


		// uniform texture sampler
		const int m_samplers[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };



	};

}