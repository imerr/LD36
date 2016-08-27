#include "LD36.hpp"
#include "Level.hpp"

#include <Engine/Factory.hpp>

LD36::LD36(): Game(1024, 576), m_currentLevel(0) {
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

void LD36::Restart() {
	Level* level = engine::Factory::create<Level>("assets/scripts/level.json", this);
	level->SetPyramid(m_currentLevel);
	auto ufo = level->GetChildByID("ufo");
	level->DestroyParticles(ufo);
	level->MakePart(ufo);
	m_loadingScene.Switch(m_scene, level);
	m_scene = &m_loadingScene;
}

void LD36::Next() {
	m_currentLevel++;
	Restart();
}
