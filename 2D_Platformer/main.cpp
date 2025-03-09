#include "stdafx.h"
#include "game.h"

int main(){
	srand(static_cast<unsigned>(time(0)));
	Game game;
	while (game.getWindow().isOpen()) {
		game.update();
		game.render();
	}

	return 0;
}