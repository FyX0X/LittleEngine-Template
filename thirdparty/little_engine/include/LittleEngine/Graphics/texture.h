#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "LittleEngine/internal.h"
#include <string>

namespace LittleEngine
{
#pragma region Texture

	class Texture
	{
	public:
		GLuint id = 0;

		void LoadFromFile(const std::string& path, bool pixelated = defaults::pixelated, bool mipmaps = defaults::mipmaps, bool verticalFlip = false);
		void LoadFromData(const unsigned char* data, int width, int height, int channelCount = 4, bool pixelated = defaults::pixelated, bool mipmaps = defaults::mipmaps, bool verticalFlip = false);
		//glm::ivec2 GetSize();


		void Bind(const unsigned int sample = 0) const;
		void Unbind() const;

		void Cleanup();

		static Texture GetDefaultTexture();
	private:
		static void CreateDefaultTexture();
		static void FlipBitmapVertically(unsigned char* bitmap, int width, int height, int pixelSize);
		static GLuint s_defaultTexId;
	};
#pragma endregion



#pragma region TextureAtlas
	struct AtlasCoord
	{
		int x;
		int y;
	};

	struct TextureAtlas
	{
		TextureAtlas() {};
		TextureAtlas(int xCount, int yCount) :xCountInv(1.f/xCount), yCountInv(1.f/yCount) {};

		float xCountInv = 0;
		float yCountInv = 0;

		// Returns UV coords of block in (u0, v0, u1, v1) format.
		glm::vec4 GetUV(AtlasCoord coord)
		{
			return GetUV(coord.x, coord.y);
		}
		glm::vec4 GetUV(int x, int y);


	};



}