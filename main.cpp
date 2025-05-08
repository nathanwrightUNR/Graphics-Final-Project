#include <iostream>

#include "engine.h"

/**
 * @brief
 * main fn instantiates engine class with window params
 * and then runs, and cleans up.
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv)
{

	// instantiate the engine, with window name w and h.
	Engine *engine = new Engine("Wright-PA5", 800, 600);

	// if initialization fails, print error message and clean up.
	if (!engine->Initialize())
	{
		printf("The engine failed to start.\n");
		delete engine;
		engine = NULL;
		return 1;
	}

	// run the engine.
	engine->Run();

	// delete the engine and set to null.
	delete engine;
	engine = NULL;

	return 0;
}
