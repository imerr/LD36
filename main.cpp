#include "LD36.hpp"
#include "Ufo.hpp"
#include <Engine/Factory.hpp>

int main() {
	engine::Factory::RegisterType("ufo", engine::Factory::CreateChildNode<Ufo>);

	LD36 game;
	game.run();
	return 0;
}