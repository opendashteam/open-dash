#pragma once

/*
This is the game entry code which is
to be implemented by the game.
*/
namespace game
{

/*
Because different platforms can have different types of main functions,
the main function of the respective platform is written in src/platform.
That main function then calls this function. If it returns false, the
game closes before starting up usually due to a problem.
*/
bool init();

/*
The platform code has to handle the game loop themselves and also has to
call this loop function in it. If this loop function returns false, the
game quits and runs the quit function below. However, if the platform
gets a signal to close the game. It can call the quit function below
no matter if loop returned true or false.
*/
bool loop();

void quit();

};