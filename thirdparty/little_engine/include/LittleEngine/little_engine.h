//////////////////////////////////////////////////
// LittleEngine 1.0
// author: Martin Ergo
//////////////////////////////////////////////////

#pragma once

//enable simd functions
//set LittleEngine_SIMD to 0 if it doesn't work on your platform
#ifdef _WIN32
#define LittleEngine_SIMD 1
#else
#define LittleEngine_SIMD 0
#endif


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <random>
#include <stb_image/stb_image.h>
#include <vector>
#include <functional>

#include "LittleEngine/Graphics/camera.h"
#include "LittleEngine/Graphics/color.h"
#include "LittleEngine/Graphics/font.h"
#include "LittleEngine/Graphics/renderer.h"
#include "LittleEngine/Graphics/shader.h"
#include "LittleEngine/Graphics/texture.h"
#include "LittleEngine/Graphics/tilemap_renderer.h"
#include "LittleEngine/Input/input.h"

#include "LittleEngine/internal.h"



/**		TODO LIST:
 *
 * GRAPHICS:
 * 
 * add render target and fbo.
 * draw line
 * draw circle ?
 * draw particle ?
 * optimise renderer flush and batching.
 * better text rendering. (shadow, higlight)
 *	
 * INPUT:
 * 
 * stored keys ok
 * action ok
 * callbacks ok
 * 
 * scroll
 * mouse get pos 
 * 
 * 
 * AUDIO:
 * 
 * everything
 * 
 * 
 */


namespace LittleEngine
{

	struct EngineConfig
	{
		std::string title = "LittleEngine Game";
		int windowWidth = 800;
		int windowHeight = 600;
		bool vsync = true;
	};


	//Initializes the library. Call once before you use the library.
	int Initialize(const EngineConfig& config = {});
	
	// Calls the main loop function from the game
	void Run(const std::function<void(float)>& update, const std::function<void()>& render);

	//Deinitializes the library.
	void Shutdown();

	// Gets the viewport size.
	glm::ivec2 GetWindowSize();


	using ResizeCallback = std::function<void(int width, int height)>;
	void SetWindowResizeCallback(ResizeCallback callback);


	void SetVsync(bool b);
	void SetWireframe(bool b);



#pragma endregion



};