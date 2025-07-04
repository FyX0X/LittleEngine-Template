#pragma once

#include "LittleEngine/Graphics/texture.h"


#include <ft2build.h>
#include FT_FREETYPE_H

#include <unordered_map>

namespace LittleEngine
{
	struct GlyphInfo {
		glm::vec2 size;        // width, height in pixels
		glm::vec2 bearing;     // offset from baseline to top-left
		glm::vec4 uv;			// texture coordinates in atlas
		float advance;         // advance in pixels
	};

	
	class Font
	{
	public:


		static void Initialize();

		void LoadFromTTF(const std::string& path, int size, bool pixelated = false);	
		void LoadFromData(const unsigned char* data, int dataSize, float size, bool pixelated = false);

		//const stbtt_bakedchar* GetGlyphs() const { return m_chars; }
		//const void Font::GetQuad(char c, float& xpos, float& ypos, stbtt_aligned_quad& q) const;
		const Texture& GetTexture() const {	return m_texture; }
		const GlyphInfo& GetGlyph(char c) const;
		const std::unordered_map<char, GlyphInfo>& GetGlyphs() const { return m_glyphMap; }

		static Font GetDefaultFont(float size);

		void Bind() const { m_texture.Bind(); }
		void Unbind() const { m_texture.Unbind(); }
		float GetSize() const { return m_size; }

	private:

		void GenerateAtlas(bool pixelated = false);

		static FT_Library s_ft;
		static bool s_initialized;
		FT_Face m_face;
		Texture m_texture;
		// Pack glyphs into atlas and store their info
		std::unordered_map<char, GlyphInfo> m_glyphMap;
		float m_size = 0;


	};


}