#define GLM_ENABLE_EXPERIMENTAL
#include "game.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#if ENABLE_IMGUI == 1
#include "imgui.h"
#endif

#include <iostream>
#include <sstream>

#include <LittleEngine/little_engine.h>

#include <LittleEngine/error_logger.h>




#include <iostream>
#include <iomanip>
#include <sstream>



namespace game
{

	const unsigned int Game::world[] = { 2, 2, 2, 3, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 3, 3, 2, 3, 2, 2, 2, 2, 2, 2, 3, 3, 2, 2, 2, 2, 2, 3, 3, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

#pragma region Game Initialization/Unintialization

	bool Game::Initialize()
	{
		m_renderer = std::make_unique<LittleEngine::Graphics::Renderer>();
		m_renderer->Initialize(sceneCamera, LittleEngine::GetWindowSize());

		m_audioSystem = std::make_unique<LittleEngine::Audio::AudioSystem>();
		m_audioSystem->Initialize();


		// load resources


		m_audioSystem->LoadSound(RESOURCES_PATH "test.wav", sound);
		texture1.LoadFromFile(RESOURCES_PATH "test.jpg");
		texture2.LoadFromFile(RESOURCES_PATH "awesomeface.png", true, false, true);
		torch.LoadFromFile(RESOURCES_PATH "torch.png");
		minecraft_blocks.LoadFromFile(RESOURCES_PATH "minecraft_atlas.png");
		minecraft_atlas = LittleEngine::Graphics::TextureAtlas(minecraft_blocks, 16, 16);

		glm::vec4 r = minecraft_atlas.GetUV(0, 0);
		std::cout << r.x << " " << r.y << " " << r.z << " " << r.w << "\n";
		std::cout << minecraft_atlas.textureWidth << " " << minecraft_atlas.textureHeight << " " << minecraft_atlas.cellWidth << " " << minecraft_atlas.cellHeight << "\n";

		font.LoadFromTTF(RESOURCES_PATH "arial.ttf", 64.f);

		//for (size_t i = 0; i < 100; i++)
		//{
		//	LittleEngine::Texture tex;
		//	tex.LoadFromFile(RESOURCES_PATH "wall.jpg");
		//	textures.push_back(tex);
		//}


		// Creating tilemap
		//				0		1		2		3
		tileIDs = { {3, 15}, {2, 15}, {1, 15}, {0, 15} };

		tilemap.SetTileSetTexture(minecraft_blocks, minecraft_atlas);
		//tilemap.SetTileSize(1.f);
		tilemap.SetTileSetAtlasKey(tileIDs);
		tilemap.SetMap(world, 10, 10, { 0, 0 });


		// initialize the blur shader

		blurShader.Create(RESOURCES_PATH "fullscreen_quad.vert", RESOURCES_PATH "blur.frag", true);
		lightSceneMergingShader.Create(RESOURCES_PATH "fullscreen_quad.vert", RESOURCES_PATH "merge_light_scene.frag", true);


		// initialize the render targets
		ResizeFBOs();
		
		// create custom texture with render target

		sceneCamera.centered = true;	// center camera on screen
		sceneCamera.viewportSize = LittleEngine::GetWindowSize();
		UICamera.viewportSize = LittleEngine::GetWindowSize();

		target.Create(100, 100);
		m_renderer->SetRenderTarget(&target);

		LittleEngine::Graphics::Camera tempCamera = UICamera;
		tempCamera.viewportSize = (glm::vec2)target.GetSize();
		m_renderer->SetCamera(tempCamera);

		m_renderer->DrawRect({ 0, 0, 100, 100 }, texture2);
		m_renderer->DrawString("target", { 10, 100 }, LittleEngine::Graphics::Colors::White, 10);

		m_renderer->Flush();
		m_renderer->SetRenderTarget();

		m_renderer->SetCamera(sceneCamera);
		


		for (int i = 0; i < 100; i++)
		{
			glm::vec4 r = { 0, 0, 0.8f, 0.8f };
			r.x = (i % 10) - 5;     // x goes from -5 to +4 across columns
			r.y = (i / 10) - 5;     // y goes from -5 to +4 across rows
			rect.push_back(r);
			if (i >= 90)
			{
				rect_uv.push_back(minecraft_atlas.GetUV(3, 15));
			}
			else
			{
				rect_uv.push_back(minecraft_atlas.GetUV(2, 15));
			}
		}


		// create axis
		//LittleEngine::Input::RegisterAxis("vertical");
		//LittleEngine::Input::RegisterAxis("horizontal");
		//LittleEngine::Input::BindKeysToAxis(GLFW_KEY_W, GLFW_KEY_S, "vertical");
		//LittleEngine::Input::BindKeysToAxis(GLFW_KEY_UP, GLFW_KEY_DOWN, "vertical");
		//LittleEngine::Input::BindKeysToAxis(GLFW_KEY_D, GLFW_KEY_A, "horizontal");
		//LittleEngine::Input::BindKeysToAxis(GLFW_KEY_RIGHT, GLFW_KEY_LEFT, "horizontal");

		// Register two separate axes
		LittleEngine::Input::RegisterAxis("vertical");
		LittleEngine::Input::RegisterAxis("horizontal");

		// Primary keys for each axis
		LittleEngine::Input::BindKeysToAxis(GLFW_KEY_W, GLFW_KEY_S, "vertical");
		LittleEngine::Input::BindKeysToAxis(GLFW_KEY_D, GLFW_KEY_A, "horizontal");

		// Duplicate (secondary) keys for each axis
		LittleEngine::Input::RegisterAxis("vertical_alt");
		LittleEngine::Input::RegisterAxis("horizontal_alt");

		LittleEngine::Input::BindKeysToAxis(GLFW_KEY_UP, GLFW_KEY_DOWN, "vertical_alt");
		LittleEngine::Input::BindKeysToAxis(GLFW_KEY_RIGHT, GLFW_KEY_LEFT, "horizontal_alt");


#pragma region Command definitions

		class ZoomCommand : public LittleEngine::Input::Command {
			float& zoom;
		public:
			ZoomCommand(float& zoom) : zoom(zoom) {}
			std::string GetName() const override { return "Zoom"; }
			
			void OnPress() override { zoom *= 2.f; }
			void OnHold() override {
				zoom *= 0.99f;
				if (zoom <= 1)
					zoom = 1.f;
			}
			void OnRelease() override {}
		};

		class JumpCommand : public LittleEngine::Input::Command {
			glm::vec2& pos;
		public:
			JumpCommand(glm::vec2& pos) : pos(pos) {}
			std::string GetName() const override { return "Jump"; }

			void OnPress() override { pos.y += 2.f; }
			void OnRelease() override { pos.y -= 2.f; }
			void OnHold() override {}
		};

		class ScreenshotCommand : public LittleEngine::Input::Command {
			LittleEngine::Graphics::Renderer* renderer;
		public:
			ScreenshotCommand(LittleEngine::Graphics::Renderer* r) : renderer(r) {}
			std::string GetName() const override { return "Screenshot"; }

			void OnPress() override { renderer->SaveScreenshot(); }
		};

		class SaveRenderTargetCommand : public LittleEngine::Input::Command {
			LittleEngine::Graphics::Renderer* renderer;
			LittleEngine::Graphics::RenderTarget& target;
		public:
			SaveRenderTargetCommand(LittleEngine::Graphics::Renderer* r,
				LittleEngine::Graphics::RenderTarget& t) : renderer(r), target(t) {}
			
			std::string GetName() const override { return "SaveRenderTarget"; }

			void OnPress() override { 
				std::cout << "hello before: ";
				renderer->SaveScreenshot(&target);
				std::cout << "hello after\n ";
			}
		};


		class ColorCommand : public LittleEngine::Input::Command {
			LittleEngine::Graphics::Color& color;
		public:
			ColorCommand(LittleEngine::Graphics::Color& c) : color(c) {}
			std::string GetName() const override { return "Color"; }

			void OnPress() override { 
				float f = (rand() % 1000) / 1000.f;
				color = f * LittleEngine::Graphics::Colors::Green;
				color.w = 1.f;
			}
			void OnRelease() override {}
			void OnHold() override {}
		};

		class SoundCommand : public LittleEngine::Input::Command {
			LittleEngine::Audio::Sound& sound;
		public:
			SoundCommand(LittleEngine::Audio::Sound& s) : sound(s) {}
			std::string GetName() const override { return "Sound"; }

			void OnPress() override {

				// play sound
				sound.Play();
			}
			void OnRelease() override {}
			void OnHold() override {}
		};

#pragma endregion


		LittleEngine::Input::BindKeyToCommand(GLFW_KEY_SPACE, std::make_unique<ColorCommand>(m_data.color));
		//LittleEngine::Input::BindKeyToCommand(GLFW_KEY_SPACE, std::make_unique<JumpCommand>(m_data.rectPos));
		LittleEngine::Input::BindKeyToCommand(GLFW_KEY_SPACE, std::make_unique<SoundCommand>(sound));
		LittleEngine::Input::BindMouseButtonToCommand(GLFW_MOUSE_BUTTON_LEFT, std::make_unique<ColorCommand>(m_data.color));
		LittleEngine::Input::BindMouseButtonToCommand(GLFW_MOUSE_BUTTON_RIGHT, std::make_unique<ZoomCommand>(m_data.zoom));
		LittleEngine::Input::BindKeyToCommand(GLFW_KEY_F11, std::make_unique<ScreenshotCommand>(m_renderer.get()));
		LittleEngine::Input::BindKeyToCommand(GLFW_KEY_F10, std::make_unique<SaveRenderTargetCommand>(m_renderer.get(), target));



		//loading the saved data. Loading an entire structure like this makes savind game data very easy.
		//platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

		return true;
	}


	void Game::Shutdown()
	{
		m_renderer->Shutdown();
		m_audioSystem->Shutdown();
		sound.Shutdown();

		//saved the data.
		// platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));
	}


#pragma endregion

#pragma region Mainloop

	void Game::Update(float dt)
	{
		delta = dt;


		// check axis input.

		float vert = LittleEngine::Input::GetAxis("vertical");
		float hori = LittleEngine::Input::GetAxis("horizontal");

		glm::vec2 move = { hori, vert };
		float length = glm::length(move);
		if (length > 1)
			move /= length;

		m_data.rectPos += move * speed * dt;

		vert = LittleEngine::Input::GetAxis("vertical_alt");
		hori = LittleEngine::Input::GetAxis("horizontal_alt");

		move = { hori, vert };
		length = glm::length(move);
		if (length > 1)
			move /= length;

		m_data.pos2 += move * 10.f * dt;



		//m_renderer->camera.Follow(m_data.rectPos, dt, cameraFollowSpeed, maxDist, minDist);
		sceneCamera.FollowSpring(m_data.rectPos, dt, maxDist, cameraFollowSpeed);
		sceneCamera.zoom = m_data.zoom;

		m_audioSystem->SetListenerPosition(m_data.rectPos.x, m_data.rectPos.y);


	}


	void Game::Render()
	{
#pragma region Game Rendering



		// render scene to fbo
		m_renderer->SetRenderTarget(&sceneFBO);
		sceneFBO.Clear();

#pragma region Scene Render
		m_renderer->BeginFrame();

		// green block from (-10, -10) to (0 0)
		m_renderer->DrawRect({ -10, -10, 10 , 10 }, m_data.color);

		// font block from (0, 0) to (15 15)
		//m_renderer->DrawRect({ 0, 0, 15 , 15 }, font.GetTexture());


		//m_renderer->DrawRect({ -15, 10, 15 , 15 }, texture2);



		for (int i = 0; i < rect.size(); i++)
		{
			m_renderer->DrawRect(rect[i], minecraft_blocks, color, rect_uv[i]);
			//m_renderer->DrawRect(rect[i], textures[i], color);
		}

		m_renderer->DrawString("Some Text. ) ! 123", { -9.97, 4.97 }, font, LittleEngine::Graphics::Colors::Gray, scale);
		m_renderer->DrawString("Some Text. ) ! 123", { -10, 5 }, font, LittleEngine::Graphics::Colors::White, scale);

		m_renderer->DrawString("Hello Arial", { 0, 0 }, font, LittleEngine::Graphics::Colors::White, scale);
		m_renderer->DrawString("Hello Default", { 0, -3 }, LittleEngine::Graphics::Colors::White, scale);

		for (size_t i = 0; i < length; i++)
		{
			tilemap.Draw(m_renderer.get());		// each call draws the whole timeMap, only for benchmark purposes
		}



		m_renderer->DrawRect(glm::vec4(m_data.pos2, 3.f, 3.f), target.GetTexture());

		m_renderer->Flush();

#pragma endregion


#pragma region Light Render

		m_renderer->SetRenderTarget(&lightFBO);
		m_renderer->BeginFrame();
		//lightFBO.Clear({0.1,0.1,0.2,1});
		lightFBO.Clear(LittleEngine::Graphics::Colors::Black);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE); // Additive

		// use awesomeface.png as light texture
		m_renderer->DrawRect(glm::vec4(m_data.pos2, 3.f, 3.f), target.GetTexture());

		m_renderer->DrawRect(glm::vec4(1.f, 1.f, 1.f, 1.f), LittleEngine::Graphics::Colors::Red * lightIntensity);


		//m_renderer->SaveScreenshot(&lightFBO, "before");
		m_renderer->Flush();
		//m_renderer->SaveScreenshot(&lightFBO, "flush");

		// restore default blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		BlurLightTexture(lightFBO, blurPasses, blurShader);
		//m_renderer->SaveScreenshot(&lightFBO, "blur");




#pragma endregion

		m_renderer->SetRenderTarget();

		lightSceneMergingShader.Use();

		lightSceneMergingShader.SetInt("sceneTexture", 0);
		lightSceneMergingShader.SetInt("lightTexture", 1);

		sceneFBO.GetTexture().Bind(0);
		lightFBO.GetTexture().Bind(1);

		m_renderer->FlushFullscreenQuad();


		m_renderer->shader.Use();




#pragma endregion



#pragma region ImGui Render
#if ENABLE_IMGUI == 1
 		float fps = 1.f / delta;
		// Your ImGui widgets here
		ImGui::Begin("Debug");
		ImGui::Text("FPS: %.2f", fps);
		ImGui::Text("QuadCount: %d", m_renderer->GetQuadCount());
		ImGui::Text("DATA: %.1f, %.1f", m_data.rectPos.x, m_data.rectPos.y);
		ImGui::SliderFloat("Camera Zoom", &m_data.zoom, 0.1f, 100.f);
		ImGui::SliderFloat("light intensity", &lightIntensity, 0.1f, 100.f);
		ImGui::SliderInt("Blur passes", &blurPasses, 1, 20);
		if (ImGui::SliderInt("Downscale Factor", &downscaleFactor, 1, 20))
		{
			// resize the render targets
			ResizeFBOs();
		}
		//ImGui::SliderFloat("Camera x", &m_data.rectPos.x, -50.f, 50.f);
		//ImGui::SliderFloat("Camera y", &m_data.rectPos.y, -50.f, 50.f);
		//ImGui::SliderFloat("Red", &color.x, 0.f, 1.f);
		//ImGui::SliderFloat("Green", &color.y, 0.f, 1.f);
		//ImGui::SliderFloat("Blue", &color.z, 0.f, 1.f);
		//ImGui::SliderFloat("Alpha", &color.w, 0.f, 1.f);
		// 
		// SOUND
		// 
		//if (ImGui::SliderFloat("min dist", &minD, 0.f, 20.f))
		//{
		//	sound.SetMinDistance(minD);
		//}
		//if (ImGui::SliderFloat("max dist", &maxD, 0.f, 20.f))
		//{
		//	sound.SetMaxDistance(maxD);
		//}		
		//if (ImGui::SliderFloat("min gain", &minG, 0.f, 1.f))
		//{
		//	sound.SetMinGain(minG);
		//}
		//if (ImGui::SliderFloat("max gain", &maxG, 0.f, 1.f))
		//{
		//	sound.SetMaxGain(maxG);
		//}
		//if (ImGui::SliderFloat("rolloff", &rolloff, 0.f, 10.f))
		//{
		//	sound.SetRolloff(rolloff);
		//}		
		// if (ImGui::SliderFloat("pitch", &pitch, 0.f, 10.f))
		//{
		//	sound.SetPitch(pitch);
		//}
		//if (ImGui::SliderFloat("volume", &volume, 0.f, 10.f))
		//{
		//	sound.SetVolume(volume);
		//}
		ImGui::SliderFloat("scale", &scale, 0.1f, 5.f);
		ImGui::SliderFloat("speed", &speed, 0.f, 100.f);
		ImGui::SliderFloat("camera follow speed", &cameraFollowSpeed, 0.f, 30.f);
		ImGui::SliderFloat("max camera dist", &maxDist, 0.f, 10.f);
		ImGui::SliderFloat("min camera dist", &minDist, 0.f, 1.f);

		ImGui::SliderInt("Tilemap Count", &length, 0, 200);

		if (ImGui::Checkbox("wireframe", &w))
		{
			LittleEngine::SetWireframe(w);
		}
		if (ImGui::Checkbox("vsinc", &v))
		{
			LittleEngine::SetVsync(v);
		}
		if (ImGui::Checkbox("spatialized", &spatialized))
		{
			sound.SetSpatialization(spatialized);
		}

		// fps graph
		if (fpsHistory.size() >= historySize)
		{
			fpsHistory.erase(fpsHistory.begin());
		}
		fpsHistory.push_back(fps);

		float maxfps = 0;
		for (float f : fpsHistory)
		{
			if (f > maxfps)
				maxfps = f;
		}

		// Plot the FPS graph
		ImGui::PlotLines("FPS Graph", fpsHistory.data(), fpsHistory.size(), 0,
			nullptr, 0.0f, maxfps * 1.5f, ImVec2(0, 80));

		// camera distance
		if (distHistory.size() >= historySize)
		{
			distHistory.erase(distHistory.begin());
		}
		distHistory.push_back(sceneCamera.position.x - m_data.rectPos.x);

		float maxx = 0;
		for (float f : distHistory)
		{
			if (f > maxx)
				maxx = f;
			if (-f > maxx)
				maxx = -f;
		}

		// Plot the FPS graph
		ImGui::PlotLines("Delta x Graph", distHistory.data(), distHistory.size(), 0,
			nullptr, -maxx, maxx * 1.2f, ImVec2(0, 80));


		ImGui::End();

#endif
#pragma endregion



	}


#pragma endregion

#pragma region Callbacks

	void Game::OnWindowSizeChange(int w, int h)
	{
		m_renderer->UpdateWindowSize(w, h);

		// update the camera
		sceneCamera.viewportSize = glm::vec2(w, h);
		UICamera.viewportSize = glm::vec2(w, h);

		ResizeFBOs();
	}

#pragma endregion

	void Game::ResizeFBOs()
	{
		// resize the render targets
		sceneFBO.Cleanup();
		lightFBO.Cleanup();

		sceneFBO.Create(LittleEngine::GetWindowSize().x, LittleEngine::GetWindowSize().y, GL_RGB);
		lightFBO.Create(LittleEngine::GetWindowSize().x / downscaleFactor, LittleEngine::GetWindowSize().y / downscaleFactor, GL_RGB16F);

	}
	void Game::BlurLightTexture(LittleEngine::Graphics::RenderTarget& lightFBO, int passes, LittleEngine::Graphics::Shader& shader)
	{
		bool horizontal = true;
		bool firstIteration = true;

		// 1. Create two ping-pong FBOs
		LittleEngine::Graphics::RenderTarget pingpongFBO[2];
		glm::ivec2 size = lightFBO.GetSize();

		for (int i = 0; i < 2; ++i)
			pingpongFBO[i].Create(size.x, size.y); // should use RGB16F if possible

		// 2. Blur Passes
		shader.Use();

		shader.SetInt("image", 0);

		for (int i = 0; i < passes * 2; ++i) {
			pingpongFBO[horizontal].Bind();
			shader.SetBool("horizontal", horizontal);
			shader.SetFloat("texelSize", horizontal ? (1.f / size.x) : (1.f / size.y));

			if (firstIteration)
				lightFBO.GetTexture().Bind(0);
			else
				pingpongFBO[!horizontal].GetTexture().Bind(0);


			m_renderer->FlushFullscreenQuad();

			horizontal = !horizontal;
			if (firstIteration)
				firstIteration = false;
		}

		// 3. Final pass: write blurred result back into lightFBO
		lightFBO.Bind();  // bind original light FBO as target
		shader.SetBool("horizontal", false); // doesn't matter, we just draw the final result
		shader.SetFloat("texelSize", 1.f/ size.y); // doesn't matter, we just draw the final result
		pingpongFBO[!horizontal].GetTexture().Bind(0); // final blurred texture

		shader.SetInt("image", 0);

		m_renderer->FlushFullscreenQuad();

		// 4. Cleanup temporary FBOs
		for (int i = 0; i < 2; ++i)
			pingpongFBO[i].Cleanup();

		lightFBO.Unbind(); // unbind the light FBO to restore default framebuffer

		m_renderer->shader.Use(); // restore default shader
	}

}

