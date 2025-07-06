#include "LittleEngine/Input/input.h"

#include "LittleEngine/error_logger.h"



#if ENABLE_IMGUI == 1
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#endif // 



#include <unordered_map>
#include <memory>

namespace LittleEngine::Input
{


	// one vector<Command> for each key. allows for multiple command per key.
	static std::vector<std::vector<std::unique_ptr<Command>>> s_keyCommandLookup;
	static std::vector<std::vector<std::unique_ptr<Command>>> s_mouseButtonCommandLookup;


	// Keyboard states
	static std::vector<bool> s_currentKeyState;
	static std::vector<bool> s_previousKeyState;


	// Mouse button states
	static std::vector<bool> s_currentMouseButtonState;
	static std::vector<bool> s_previousMouseButtonState;

	static int s_mouseX, s_mouseY;

	// virtual axis
	// TODO CHANGE FOR VECTOR OR CHANGE REST FOR MAP
	static std::unordered_map<KeyCode, std::string> s_posKeyMap;
	static std::unordered_map<KeyCode, std::string> s_negKeyMap;
	static std::unordered_map<std::string, InputAxis> s_axices;



	void Initialize(GLFWwindow* window)
	{

		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetScrollCallback(window, scroll_callback);

		ResetKeyState();

		s_keyCommandLookup.resize(GLFW_KEY_LAST);
		s_mouseButtonCommandLookup.resize(GLFW_MOUSE_BUTTON_LAST);

	}

	void ResetKeyState()
	{
		s_previousKeyState.resize(GLFW_KEY_LAST, false);
		s_currentKeyState.resize(GLFW_KEY_LAST, false);
		s_previousMouseButtonState.resize(GLFW_MOUSE_BUTTON_LAST, false);
		s_currentMouseButtonState.resize(GLFW_MOUSE_BUTTON_LAST, false);
	}

	void Shutdown()
	{
	}




	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
#if ENABLE_IMGUI == 1
		// callback for imgui
		ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

#endif
		// xoffset and yoffset indicate scroll amount (e.g. vertical scroll in yoffset)
		LogWarning("Input::scroll_callback not implemented yet.");
	}

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
#if ENABLE_IMGUI == 1
		// callback for imgui
		ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

#endif
		// xpos and ypos are the new mouse cursor position in window coordinates
		// where (0, 0) is the top left corner and measured in pixels.

		s_mouseX = static_cast<int>(xpos);
		s_mouseY = static_cast<int>(ypos);
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
#if ENABLE_IMGUI == 1
		// callback for imgui
		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);  // Forward to ImGui

#endif
		if (button < 0 || button >= GLFW_MOUSE_BUTTON_LAST) return; // GLFW_MOUSE_BUTTON_LAST = 7

		if (action == GLFW_PRESS)
			s_currentMouseButtonState[button] = true;
		else if (action == GLFW_RELEASE)
			s_currentMouseButtonState[button] = false;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
#if ENABLE_IMGUI == 1
		// callback for imgui
		ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);  // Forward to ImGui

#endif
		if (key < 0 || key >= GLFW_KEY_LAST) return; // GLFW_KEY_LAST = 348

		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			s_currentKeyState[key] = true;
		else if (action == GLFW_RELEASE)
			s_currentKeyState[key] = false;
	}

	void UpdateInputState()
	{
		
		// reset all axis;
		for (auto& [key, value] : s_axices)
		{
			value = 0.f;
		}


		// Keys
		for (int key = 0; key < s_keyCommandLookup.size(); ++key)
		{
			bool current = s_currentKeyState[key];
			bool previous = s_previousKeyState[key];
			
			// update axis.
			if (current)
			{
				auto it = s_posKeyMap.find(key);
				if (it != s_posKeyMap.end()) // found
				{
					s_axices[it->second] += 1.f;
				}
				it = s_negKeyMap.find(key);
				if (it != s_negKeyMap.end()) // found
				{
					s_axices[it->second] -= 1.f;
				}
			}

			// call key press, release, hold callbacks
			if (key >= s_keyCommandLookup.size()) continue;

			std::vector<std::unique_ptr<Command>>& cmds = s_keyCommandLookup[key];
			if (cmds.empty()) continue;


			InputEventType type = GetInputEventType(previous, current);


			for (size_t i = 0; i < cmds.size(); i++)
			{
				CallCommand(cmds[i].get(), type);
			}


		}

		// Same for mouse buttons...
		for (int btn = 0; btn < s_mouseButtonCommandLookup.size(); ++btn)
		{
			bool current = s_currentMouseButtonState[btn];
			bool previous = s_previousMouseButtonState[btn];

			if (btn >= s_mouseButtonCommandLookup.size()) continue;

			std::vector<std::unique_ptr<Command>>& cmds = s_mouseButtonCommandLookup[btn];
			if (cmds.empty()) continue;

			InputEventType type = GetInputEventType(previous, current);
			
			for (size_t i = 0; i < cmds.size(); i++)
			{
				CallCommand(cmds[i].get(), type);
			}
		}

		// clamp axices
		for (auto& [key, value] : s_axices)
		{
			if (value > 1)
				value = 1.f;
			else if (value < -1)
				value = -1.f;
		}

		// for next frame
		s_previousKeyState = s_currentKeyState;
		s_previousMouseButtonState = s_currentMouseButtonState;

		
	}

	InputEventType GetInputEventType(bool previous, bool current)
	{
		if (!previous && current)
			return InputEventType::Pressed;

		if (previous && current)
			return InputEventType::Down;

		if (previous && !current)
			return InputEventType::Released;

		return InputEventType::Up;

	}

	void CallCommand(Command* cmd, InputEventType type)
	{

		if (cmd == nullptr) 
		{
			LogError("Input::CallCommand: Command is nullptr.");
			return;
		}

		switch (type)
		{
		case LittleEngine::Input::InputEventType::Pressed:
			cmd->OnPress();
			break;
		case LittleEngine::Input::InputEventType::Released:
			cmd->OnRelease();
			break;
		case LittleEngine::Input::InputEventType::Down:
			cmd->OnHold();
			break;
		default:
			//LogWarning("Input::CallAction: unknown input event type.");
			break;
		}
	
	}

	float GetAxis(const std::string& name)
	{
		auto it = s_axices.find(name);
		if (it != s_axices.end())	// found
		{
			return it->second;
		}
		LogError("Input::GetAxis: No axis with name: " + name);
		return 0;

	}

#pragma region helper functions.



	bool IsKeyDown(KeyCode key) 
	{
		return s_currentKeyState[key];
	}

	bool IsKeyPressed(KeyCode key) 
	{
		return s_currentKeyState[key] && !s_previousKeyState[key];
	}

	bool IsKeyReleased(KeyCode key) 
	{
		return !s_currentKeyState[key] && s_previousKeyState[key];
	}


#pragma endregion

	void BindKeyToCommand(KeyCode key, std::unique_ptr<Command> cmd)
	{
		if (key < 0 || key > GLFW_KEY_LAST)
		{
			LogError("Input::BindKeyToCommand: invalid key: " + std::to_string(key));
			return;
		}

		// check if action already binded to key.
		auto& cmds = s_keyCommandLookup[key];
		for (size_t i = 0; i < cmds.size(); i++)
		{
			if (typeid(*cmds[i]) == typeid(*cmd))		// *cmd and not cmd.get() to dereference and get Actual type (not Command)
			{
				LogError("Input::BindKeyToCommand: Command " + cmd->GetName() + " already binded to key: " + std::to_string(key));
				return;
			}
		}
		cmds.push_back(std::move(cmd));
	}

	void BindMouseButtonToCommand(MouseButton mb, std::unique_ptr<Command> cmd)
	{
		if (mb < 0 || mb > GLFW_MOUSE_BUTTON_LAST)
		{
			LogError("Input::BindMouseButtonToCommand: invalid mouse button: " + std::to_string(mb));
			return;
		}
		// check if action already binded to mouse button.
		auto& cmds = s_mouseButtonCommandLookup[mb];
		for (size_t i = 0; i < cmds.size(); i++)
		{
			if (typeid(*cmds[i]) == typeid(*cmd))
			{
				LogError("Input::BindMouseButtonToCommand: command: " + cmd->GetName() + " already binded to mouse button: " + std::to_string(mb));
				return;
			}
		}
		cmds.push_back(std::move(cmd));
	}

	void RegisterAxis(const std::string& name)
	{
		auto it = s_axices.find(name);
		if (it == s_axices.end())	// not found
		{
			s_axices[name] = 0;
		}
		else 
		{
			LogError("Input::RegisterAxis: Axis '" + name + "' already exists.");
		}
	}

	void BindKeysToAxis(KeyCode keyPositive, KeyCode keyNegative, const std::string& name)
	{

		if (s_axices.find(name) == s_axices.end())	// not found
		{
			LogError("Input::BindKeysToAxis: axis '" + name + "' does not exists.");
			return;
		}

		s_posKeyMap[keyPositive] = name;
		s_negKeyMap[keyNegative] = name;

		
	}


}