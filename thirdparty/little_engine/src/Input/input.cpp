#include "LittleEngine/Input/input.h"

#include "LittleEngine/error_logger.h"



#if ENABLE_IMGUI == 1
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#endif // 


namespace LittleEngine::Input
{


	static std::vector<ActionID> s_keyActionLookup;
	static std::vector<ActionID> s_mouseButtonActionLookup;

	static std::vector<Command> s_pressedCallbacks;
	static std::vector<Command> s_heldCallbacks;
	static std::vector<Command> s_releasedCallbacks;


	// Keyboard states
	static std::vector<bool> s_currentKeyState;
	static std::vector<bool> s_previousKeyState;

	// Mouse button states
	static std::vector<bool> s_currentMouseButtonState;
	static std::vector<bool> s_previousMouseButtonState;

	static int s_mouseX, s_mouseY;



	void Initialize(GLFWwindow* window)
	{

		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetScrollCallback(window, scroll_callback);

		ResetKeyState();

		s_keyActionLookup.resize(GLFW_KEY_LAST, -1);
		s_mouseButtonActionLookup.resize(GLFW_MOUSE_BUTTON_LAST, -1);

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

		// Keys
		for (int key = 0; key < s_keyActionLookup.size(); ++key)
		{
			bool current = s_currentKeyState[key];
			bool previous = s_previousKeyState[key];

			if (key >= s_keyActionLookup.size()) continue;

			ActionID action = s_keyActionLookup[key];
			if (action == -1) continue;

			// Pressed
			if (!previous && current && action < s_pressedCallbacks.size() && s_pressedCallbacks[action])
				s_pressedCallbacks[action]();

			// Held
			if (previous && current && action < s_heldCallbacks.size() && s_heldCallbacks[action])
				s_heldCallbacks[action]();

			// Released
			if (previous && !current && action < s_releasedCallbacks.size() && s_releasedCallbacks[action])
				s_releasedCallbacks[action]();
		}

		// Same for mouse buttons...
		for (int btn = 0; btn < s_mouseButtonActionLookup.size(); ++btn)
		{
			bool current = s_currentMouseButtonState[btn];
			bool previous = s_previousMouseButtonState[btn];

			if (btn >= s_mouseButtonActionLookup.size()) continue;

			ActionID action = s_mouseButtonActionLookup[btn];
			if (action == -1) continue;

			if (!previous && current && action < s_pressedCallbacks.size() && s_pressedCallbacks[action])
				s_pressedCallbacks[action]();

			if (previous && current && action < s_heldCallbacks.size() && s_heldCallbacks[action])
				s_heldCallbacks[action]();

			if (previous && !current && action < s_releasedCallbacks.size() && s_releasedCallbacks[action])
				s_releasedCallbacks[action]();
		}

		// for next frame
		s_previousKeyState = s_currentKeyState;
		s_previousMouseButtonState = s_currentMouseButtonState;
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

	bool isActionPressed(ActionID action)
	{
		LogError("NYI");
		return false;
	}

	bool isActionReleased(ActionID action)
	{
		LogError("NYI");
		return false;
	}

	bool isActionHeld(ActionID action)
	{
		LogError("NYI");
		return false;
	}

#pragma endregion

	void BindKeyToAction(KeyCode key, ActionID action)
	{
		if (key < 0 || key > GLFW_KEY_LAST)
		{
			LogError("Input::BindKeyToAction: invalid key: " + std::to_string(key));
			return;
		}
		s_keyActionLookup[key] = action;
	}

	void BindMouseButtonToAction(MouseButton mb, ActionID action)
	{
		if (mb < 0 || mb > GLFW_MOUSE_BUTTON_LAST)
		{
			LogError("Input::BindMouseButtonToAction: invalid mouse button: " + std::to_string(mb));
			return;
		}
		s_mouseButtonActionLookup[mb] = action;
	}

	void BindActionToCommand(ActionID action, InputEventType type, Command cmd)
	{
		auto& callbacks = [&]() -> std::vector<Command>&{
				switch (type) {
				case InputEventType::Pressed: return s_pressedCallbacks;
				case InputEventType::Down:    return s_heldCallbacks;
				case InputEventType::Released:return s_releasedCallbacks;
				default: LogError("Input::BindActionToCommand: Unknown event type");
				}
			}();


		if (action >= callbacks.size())
			callbacks.resize(action + 1);

		callbacks[action] = std::move(cmd);
	}


}