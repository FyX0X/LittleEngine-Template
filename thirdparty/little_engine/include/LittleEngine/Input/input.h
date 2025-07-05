#pragma once

#include <unordered_map>
#include <iostream>
#include <memory>
#include <functional>
#include <GLFW/glfw3.h>


namespace LittleEngine::Input
{
	using KeyCode = int;
	using MouseButton = int;
	using ActionID = int;
	using Command = std::function<void()>;

	enum class InputEventType {
		Pressed,   // Trigger once when the key/button goes down
		Released,  // Trigger once when the key/button goes up
		Down       // Trigger continuously every frame while held
	};

	void Initialize(GLFWwindow* window);
	void Shutdown();

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void UpdateInputState();

	void ResetKeyState();

	bool IsKeyDown(KeyCode key);
	bool IsKeyPressed(KeyCode key);
	bool IsKeyReleased(KeyCode key);
	
	bool isActionPressed(ActionID action);
	bool isActionReleased(ActionID action);
	bool isActionHeld(ActionID action);

	//void HandleInput(const int inputKey);


	void BindKeyToAction(KeyCode key, ActionID action);
	void BindMouseButtonToAction(MouseButton mb, ActionID action);
	void BindActionToCommand(ActionID action, InputEventType type, Command cmd);





}