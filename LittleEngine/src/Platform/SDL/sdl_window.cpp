#include "LittleEngine/Platform/SDL/sdl_window.h"

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <stb_image/stb_image.h>

#include "LittleEngine/Utils/logger.h"
#include "LittleEngine/Utils/debug_tools.h"



namespace LittleEngine::Platform
{




	bool SdlWindow::Initialize(const WindowConfig& config)
	{
		// Tell SDL we want an OpenGL 3.3 Core context
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// Enable double buffering
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#if DEVELOPMENT_BUILD == 1
		// Enable OpenGL debug context
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif


		m_window = SDL_CreateWindow(
			config.title.c_str(),
			config.width,
			config.height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		);		

		if (!m_window)
		{
			Utils::Logger::Critical(std::string("Failed to create SDL window: ") + SDL_GetError());
			SDL_Quit();
			return false;
		}


		//if (config.maximized)	TODO 	
		//	SdlWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);


		// Create OpenGL context
		m_glContext = SDL_GL_CreateContext(m_window);
		if (!m_glContext) {
			Utils::Logger::Critical(std::string("Failed to create OpenGL context: ") + SDL_GetError());
			SDL_DestroyWindow(m_window);
			SDL_Quit();
			return false;
		}

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {

			Utils::Logger::Critical("Failed to initialize GLAD");
			SDL_GL_DestroyContext(reinterpret_cast<SDL_GLContext>(m_glContext));
			SDL_DestroyWindow(m_window);
			return false;
		}


#if DEVELOPMENT_BUILD == 1
		// Set the OpenGL debug callback
		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(Utils::DebugTools::glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
#endif

		SetVsync(true);

		// Set initial window mode
		SetWindowMode(config.mode);
		m_state.title = config.title;
		m_state.width = config.width;
		m_state.height = config.height;
		m_state.mode = config.mode;
		m_state.vsyncEnabled = config.vsyncEnabled;

		//if (config.maximized)
		//{
		//	glfwGetFramebufferSize(m_window, &m_state.width, &m_state.height);
		//}



		// Initialize callbacks
		InitializeCallbacks();


		if (!config.iconPath.empty())
		{
			SetIcon(config.iconPath);
		}


		glDisable(GL_DEPTH_TEST);	// disable depth test by default


		return true;
	}




	void SdlWindow::Close()
	{

		if (!m_window)
		{
			Utils::Logger::Warning("SdlWindow::Close: Window not initialized.");
			return;
		}
		// Cleanup
		SDL_GL_DestroyContext(reinterpret_cast<SDL_GLContext>(m_glContext));
		SDL_DestroyWindow(m_window);
		m_window = nullptr;	// reset window pointer

		m_state.Reset(); // reset window state
	}

	void SdlWindow::OnUpdate()
	{
		SDL_assert(m_window && "SdlWindow::OnUpdate: Window not initialized.");		// not checked in release mode, only in debug mode
		// TODO CHANGE THIS BCS EVENTS
		// Poll for and process events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			// Handle events here if needed, e.g., window close, input, etc.
			if (event.type == SDL_EVENT_QUIT) {
				m_shouldClose = true; // You can define a flag to track window close
			}
		}
		// Swap front and back buffers
		SDL_GL_SwapWindow(m_window);
	}



	void SdlWindow::SetWindowTitle(const std::string& title)
	{
		SDL_SetWindowTitle(m_window, title.c_str());
		m_state.title = title;
	}

	void SdlWindow::SetVsync(bool enabled)
	{
		SDL_GL_SetSwapInterval(enabled ? 1 : 0);
		m_state.vsyncEnabled = enabled;
	}

	void SdlWindow::SetWindowMode(const WindowMode& mode)
	{
		if (mode == m_state.mode)
			return; // no change

		m_state.mode = mode;
		switch (mode)
		{
		case WindowMode::ResizableWindowed:
			break;
		default:
			Utils::Logger::Warning("SdlWindow::SetWindowMode: Not yet implemented.");
			break;
		}

		
	}

	void SdlWindow::SetIcon(const std::string& path)
	{
		if (!m_window)
		{
			Utils::Logger::Warning("SdlWindow::SetIcon: Window not initialized.");
			return;
		}
		int width, height, nrChannels;
		unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

		if (pixels)
		{
			// Create an SDL_Surface from raw data
			SDL_Surface* icon = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA8888, pixels, width * 4);

			if (icon) {
				SDL_SetWindowIcon(m_window, icon);
				SDL_DestroySurface(icon);
			}

			stbi_image_free(pixels);
		}
		else
		{
			Utils::Logger::Warning("SdlWindow::SetIcon: failed to load icon: " + path);
		}
	}

#pragma region Callbacks
	bool SdlWindow::ShouldClose() const
	{
		return m_shouldClose;
	}

	void SdlWindow::SetWindowResizeCallback(Callbacks::Window::Resize callback)
	{
		SDL_assert(m_window && "SdlWindow::SetWindowResizeCallback: Window not initialized.");
		m_state.windowResizeCallback = callback;
	}

	void SdlWindow::SetWindowFocusCallback(Callbacks::Window::Focus callback)
	{
		SDL_assert(m_window && "SdlWindow::SetWindowFocusCallback: Window not initialized.");
		m_state.windowFocusCallback = callback;
	}

	void SdlWindow::SetWindowCloseCallback(Callbacks::Window::Close callback)
	{
		SDL_assert(m_window && "SdlWindow::SetWindowCloseCallback: Window not initialized.");
		m_state.windowCloseCallback = callback;
	}

	void SdlWindow::SetMouseMoveCallback(Callbacks::Mouse::Move callback)
	{
		SDL_assert(m_window && "SdlWindow::SetMouseMoveCallback: Window not initialized.");
		m_state.mouseMoveCallback = callback;
	}

	void SdlWindow::SetMouseScrollCallback(Callbacks::Mouse::Scroll callback)
	{
		SDL_assert(m_window && "SdlWindow::SetMouseScrollCallback: Window not initialized.");
		m_state.mouseScrollCallback = callback;
	}

	void SdlWindow::SetMouseButtonCallback(Callbacks::Mouse::Button callback)
	{
		SDL_assert(m_window && "SdlWindow::SetMouseButtonCallback: Window not initialized.");
		m_state.mouseButtonCallback = callback;
	}

	void SdlWindow::SetKeyCallback(Callbacks::Keyboard::Key callback)
	{
		SDL_assert(m_window && "SdlWindow::SetKeyCallback: Window not initialized.");
		m_state.keyCallback = callback;
	}

	void SdlWindow::SetCharCallback(Callbacks::Keyboard::Char callback)
	{
		SDL_assert(m_window && "SdlWindow::SetCharCallback: Window not initialized.");
		m_state.charCallback = callback;
	}


	void SdlWindow::InitializeCallbacks()
	{


	}
#pragma endregion

}