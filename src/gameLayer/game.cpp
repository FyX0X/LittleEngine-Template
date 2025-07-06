#define GLM_ENABLE_EXPERIMENTAL
#include "game.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "imgui.h"
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


	bool Game::Initialize()
	{
		m_renderer = std::make_unique<LittleEngine::Renderer>();
		m_renderer->Initialize(LittleEngine::GetWindowSize());

		m_audioSystem = std::make_unique<LittleEngine::Audio::AudioSystem>();
		m_audioSystem->Initialize();


		// load resources
		m_audioSystem->LoadSound(RESOURCES_PATH "test.wav", sound);

		texture1.LoadFromFile(RESOURCES_PATH "test.jpg");
		texture2.LoadFromFile(RESOURCES_PATH "awesomeface.png", true, false, true);
		minecraft_blocks.LoadFromFile(RESOURCES_PATH "minecraft_atlas.png");
		minecraft_atlas = LittleEngine::TextureAtlas(16, 16);

		font.LoadFromTTF(RESOURCES_PATH "arial.ttf", 64.f);


		// Creating tilemap
		//				0		1		2		3
		tileIDs = { {3, 15}, {2, 15}, {1, 15}, {0, 15} };

		tilemap.SetTileSetTexture(minecraft_blocks, minecraft_atlas);
		//tilemap.SetTileSize(1.f);
		tilemap.SetTileSetAtlasKey(tileIDs);
		tilemap.SetMap(world, 10, 10, { 0, 0 });
		


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
		LittleEngine::Input::RegisterAxis("vertical");
		LittleEngine::Input::RegisterAxis("horizontal");
		LittleEngine::Input::BindKeysToAxis(GLFW_KEY_W, GLFW_KEY_S, "vertical");
		LittleEngine::Input::BindKeysToAxis(GLFW_KEY_UP, GLFW_KEY_DOWN, "vertical");
		LittleEngine::Input::BindKeysToAxis(GLFW_KEY_D, GLFW_KEY_A, "horizontal");
		LittleEngine::Input::BindKeysToAxis(GLFW_KEY_RIGHT, GLFW_KEY_LEFT, "horizontal");




		

		// create action command map
		//LittleEngine::Input::BindActionToCommand(InputAction::MoveUp, LittleEngine::Input::InputEventType::Down,
		//	[&]() {m_data.rectPos.y += speed;});
		//LittleEngine::Input::BindActionToCommand(InputAction::MoveLeft, LittleEngine::Input::InputEventType::Down,
		//	[&]() {m_data.rectPos.x -= speed;});
		//LittleEngine::Input::BindActionToCommand(InputAction::MoveDown, LittleEngine::Input::InputEventType::Down,
		//	[&]() {m_data.rectPos.y -= speed;});
		//LittleEngine::Input::BindActionToCommand(InputAction::MoveRight, LittleEngine::Input::InputEventType::Down,
		//	[&]() {m_data.rectPos.x += speed;});



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


		class ColorCommand : public LittleEngine::Input::Command {
			LittleEngine::Color& color;
		public:
			ColorCommand(LittleEngine::Color& c) : color(c) {}
			std::string GetName() const override { return "Color"; }

			void OnPress() override { 
				float f = (rand() % 1000) / 1000.f;
				color = f * LittleEngine::Colors::Green;
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


		LittleEngine::Input::BindKeyToCommand(GLFW_KEY_SPACE, std::make_unique<ColorCommand>(m_data.color));
		LittleEngine::Input::BindKeyToCommand(GLFW_KEY_SPACE, std::make_unique<JumpCommand>(m_data.rectPos));
		LittleEngine::Input::BindKeyToCommand(GLFW_KEY_SPACE, std::make_unique<SoundCommand>(sound));
		LittleEngine::Input::BindMouseButtonToCommand(GLFW_MOUSE_BUTTON_LEFT, std::make_unique<ColorCommand>(m_data.color));
		LittleEngine::Input::BindMouseButtonToCommand(GLFW_MOUSE_BUTTON_RIGHT, std::make_unique<ZoomCommand>(m_data.zoom));










		//loading the saved data. Loading an entire structure like this makes savind game data very easy.
		//platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

		return true;
	}



	void Game::Update(float dt)
	{
#pragma region init stuff

#pragma endregion

		delta = dt;


		// check axis input.

		float vert = LittleEngine::Input::GetAxis("vertical");
		float hori = LittleEngine::Input::GetAxis("horizontal");

		glm::vec2 move = { hori, vert };
		float length = glm::length(move);
		if (length > 1)
			move /= length;

		m_data.rectPos += move * speed * dt;




		m_renderer->camera.position = m_data.rectPos;
		m_renderer->camera.zoom = m_data.zoom;


	}


	void Game::Render()
	{
#pragma region Rendering



		m_renderer->BeginFrame();

		// green block from (-10, -10) to (0 0)
		m_renderer->DrawRect({ -10, -10, 10 , 10 }, m_data.color);

		// font block from (0, 0) to (15 15)
		//m_renderer->DrawRect({ 0, 0, 15 , 15 }, font.GetTexture());


		//m_renderer->DrawRect({ -15, 10, 15 , 15 }, texture2);

		for (int i = 0; i < rect.size(); i++)
		{
			m_renderer->DrawRect(rect[i], minecraft_blocks, color, rect_uv[i]);
		}

		m_renderer->DrawString("Some Text. ) ! 123", { -9.97, 4.97 }, font, LittleEngine::Colors::Gray, scale);
		m_renderer->DrawString("Some Text. ) ! 123", { -10, 5 }, font, LittleEngine::Colors::White, scale);

		m_renderer->DrawString("Hello Arial", { 0, 0 }, font, LittleEngine::Colors::White, scale);
		m_renderer->DrawString("Hello Default\ncc", {0, -3}, LittleEngine::Colors::White, scale);

		for (size_t i = 0; i < length; i++)
		{
			tilemap.Draw(m_renderer.get());

		}


		m_renderer->EndFrame();
#pragma endregion



#pragma region ImGui Render
 		float fps = 1.f / delta;
		// Your ImGui widgets here
		ImGui::Begin("Debug");
		ImGui::Text("FPS: %.2f", fps);
		ImGui::Text("QuadCount: %d", m_renderer->GetQuadCount());
		ImGui::Text("DATA: %.1f, %.1f", m_data.rectPos.x, m_data.rectPos.y);
		ImGui::SliderFloat("Camera Zoom", &m_data.zoom, 0.1f, 3.f);
		ImGui::SliderFloat("Camera x", &m_data.rectPos.x, -50.f, 50.f);
		ImGui::SliderFloat("Camera y", &m_data.rectPos.y, -50.f, 50.f);
		ImGui::SliderFloat("Red", &color.x, 0.f, 1.f);
		ImGui::SliderFloat("Green", &color.y, 0.f, 1.f);
		ImGui::SliderFloat("Blue", &color.z, 0.f, 1.f);
		ImGui::SliderFloat("Alpha", &color.w, 0.f, 1.f);
		ImGui::SliderFloat("scale", &scale, 0.1f, 5.f);
		ImGui::SliderFloat("speed", &speed, 0.f, 100.f);
		if (ImGui::SliderFloat("pitch", &pitch, 0.f, 10.f))
		{
			sound.SetPitch(pitch);
		}
		if (ImGui::SliderFloat("volume", &volume, 0.f, 10.f))
		{
			sound.SetVolume(volume);
		}
		ImGui::SliderInt("Tilemap Count", &length, 0, 200);

		if (ImGui::Checkbox("wireframe", &w))
		{
			LittleEngine::SetWireframe(w);
		}
		if (ImGui::Checkbox("vsinc", &v))
		{
			LittleEngine::SetVsync(v);
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


		ImGui::End();


#pragma endregion



	}
	void Game::Shutdown()
	{
		m_renderer->Shutdown();
		m_audioSystem->Shutdown();
		sound.Shutdown();

	//saved the data.
	// platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));
	}

	void Game::OnWindowSizeChange(int w, int h)
	{
		m_renderer->UpdateWindowSize(w, h);
	}
}

