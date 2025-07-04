#include "LittleEngine/Graphics/texture.h"

#include "LittleEngine/error_logger.h"
#include <stb_image/stb_image.h>
#include <vector>



namespace LittleEngine
{

    GLuint Texture::s_defaultTexId = 0;

	void Texture::LoadFromFile(const std::string& path, bool pixelated, bool mipmaps, bool verticalFlip)
	{
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
        if (data)
        {
            LoadFromData(data, width, height, 4, pixelated, mipmaps, verticalFlip);

        }
        else
        {
            LogError("Texture::LoadFromFile: Failed to load texture: " + path);
        }
        stbi_image_free(data);

	}

    void Texture::LoadFromData(const unsigned char* data, int width, int height, int channelCount, bool pixelated, bool mipmaps, bool verticalFlip)
    {
        if (id != 0)
            glDeleteTextures(1, &id);
        glGenTextures(1, &id);
        Bind();
        // set the texture wrapping parameters
        if (pixelated)
        {
            if (mipmaps)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else
        {
            if (mipmaps)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        int channelType = -1;

        switch (channelCount)
        {
        default:
            LogError("Texture::LoadFromData : allowed channelCount are {1, 2, 3, 4} but " + std::to_string(channelCount) + " was found.");
            break;
        case 1:
            channelType = GL_RED;
            break;
        case 2:
            channelType = GL_RG;
            break;
        case 3:
            channelType = GL_RGB;
            break;
        case 4:
            channelType = GL_RGBA;
            break;

        }

        if (verticalFlip)
        {
            std::vector<unsigned char> flipped(data, data + width * height * channelCount);
            FlipBitmapVertically(flipped.data(), width, height, channelCount);
            glTexImage2D(GL_TEXTURE_2D, 0, channelType, width, height, 0, channelType, GL_UNSIGNED_BYTE, flipped.data());
        } 
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, channelType, width, height, 0, channelType, GL_UNSIGNED_BYTE, data);
        }

        
        if (mipmaps)
            glGenerateMipmap(GL_TEXTURE_2D);

        if (channelCount == 1)
        {
            GLint swizzleMask[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED }; // RGB = 1.0 (white), A = red channel
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        }


        Unbind();
    }



    void Texture::Bind(const unsigned int sample) const
    {
        if (sample >= defaults::MAX_TEXTURE_SLOTS)
        {
            ThrowError("Texture::Bind : sample (" + std::to_string(sample) + ") >= MAX_TEXTURE_SLOTS (" + std::to_string(defaults::MAX_TEXTURE_SLOTS) + ')');
        }

        glActiveTexture(GL_TEXTURE0 + sample);
        glBindTexture(GL_TEXTURE_2D, id);
        /*return;
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            LogError("OpenGL Error in Texture::Bind() - Code: " + std::to_string(err));
        }*/
    }

    void Texture::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Cleanup()
    {
        glDeleteTextures(1, &id);
        *this = {};
    }

    Texture Texture::GetDefaultTexture()
    {
        if (s_defaultTexId == 0)
            CreateDefaultTexture();

        Texture def;
        def.id = s_defaultTexId;
        return def;
    }

    void Texture::CreateDefaultTexture()
    {
        if (s_defaultTexId != 0)
        {
            LogError("Texture::CreateDefaultTexture : default already exists.");
            return;
        }

        glGenTextures(1, &s_defaultTexId);

        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_defaultTexId);

        // texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // generate texture
        unsigned char data[4] = { 255, 255, 255, 255 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::FlipBitmapVertically(unsigned char* bitmap, int width, int height, int pixelSize)
    {
        int stride = width * pixelSize; // 1 byte per pixel (GL_RED or GL_RGBA...)
        std::vector<unsigned char> tempRow(stride);

        for (int y = 0; y < height / 2; ++y)
        {
            unsigned char* rowTop = bitmap + y * stride;
            unsigned char* rowBottom = bitmap + (height - 1 - y) * stride;

            // swap rows
            memcpy(tempRow.data(), rowTop, stride);
            memcpy(rowTop, rowBottom, stride);
            memcpy(rowBottom, tempRow.data(), stride);
        }
    }



    glm::vec4 TextureAtlas::GetUV(int x, int y)
    {
        return glm::vec4(x * xCountInv, y * yCountInv, (x + 1) * xCountInv, (y + 1) * yCountInv);
    }

}