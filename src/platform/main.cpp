#include <LittleEngine/little_engine.h>
#include "game.h"


int main()
{

	game::Game gameInstance;

	LittleEngine::Initialize();	
	LittleEngine::SetWindowResizeCallback([&](int w, int h) {gameInstance.OnWindowSizeChange(w, h);});

	gameInstance.Initialize();

	LittleEngine::Run(
		[&](float dt) { gameInstance.Update(dt); },
		[&]() { gameInstance.Render(); }
	);

	LittleEngine::Shutdown();

	return 0;
}