#include "LD36.hpp"
#include "Level.hpp"

#include <Engine/Factory.hpp>

LD36::LD36(): Game(1024, 576) {
	m_windowTitle = "Pyramid Builder - LD36: Ancient Technology";
	Level* level;
	m_scene = level = engine::Factory::create<Level>("assets/scripts/level.json", this);
	level->SetPyramid(0);
	auto ufo = level->GetChildByID("ufo");
	level->DestroyParticles(ufo);
	level->MakePart(ufo);
}
LD36::~LD36() {

}