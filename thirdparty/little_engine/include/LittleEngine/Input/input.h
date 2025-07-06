#pragma once

#include <iostream>
#include <functional>
#include <GLFW/glfw3.h>


namespace LittleEngine::Input
{
	using KeyCode = int;
	using MouseButton = int;

	// represents a float in [-1, 1]
	using InputAxis = float;

	class Command abstract {
	public:
		virtual std::string GetName() const = 0;
		virtual void OnPress() = 0;
		virtual void OnRelease() = 0;
		virtual void OnHold() = 0;
		virtual ~Command() = default;
	};




	enum class InputEventType {
		Pressed,   // Trigger once when the key/button goes down
		Released,  // Trigger once when the key/button goes up
		Down,       // Trigger continuously every frame while held
		Up
	};


	void Initialize(GLFWwindow* window);
	void Shutdown();

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void ResetKeyState();
	void UpdateInputState();

	float GetAxis(const std::string& name);

	bool IsKeyDown(KeyCode key);
	bool IsKeyPressed(KeyCode key);
	bool IsKeyReleased(KeyCode key);


	//void HandleInput(const int inputKey);


	void BindKeyToCommand(KeyCode key, std::unique_ptr<Command> cmd);
	void BindMouseButtonToCommand(MouseButton mb, std::unique_ptr<Command> cmd);

	void RegisterAxis(const std::string& name);
	void BindKeysToAxis(KeyCode keyPositive, KeyCode keyNegative, const std::string& name);


	InputEventType GetInputEventType(bool previous, bool current);
	void CallCommand(Command* cmd, InputEventType type);



}