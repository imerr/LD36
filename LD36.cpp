#include "LD36.hpp"
#include "Level.hpp"
#include "EndScene.hpp"

#include <Engine/Factory.hpp>
#include <Engine/util/Random.hpp>

LD36::LD36(): Game(1024, 576), m_currentLevel(0) {
	m_windowTitle = "Pyramid Builder - LD36: Ancient Technology";
	Level* level;
	m_scene = level = engine::Factory::create<Level>("assets/scripts/level.json", this);
	level->SetPyramid(0);
	auto ufo = level->GetChildByID("ufo");
	level->DestroyParticles(ufo);
	level->MakePart(ufo);
	m_window.setKeyRepeatEnabled(false);
}
LD36::~LD36() {

}

void LD36::Restart(float score) {
	Level* level = engine::Factory::create<Level>("assets/scripts/level.json", this);
	if (!level->SetPyramid(m_currentLevel)) {
		delete level;
		EndScene* end = engine::Factory::create<EndScene>("assets/scripts/end.json", this, m_scores);
		m_loadingScene.Switch(m_scene, end);
	} else {
		auto ufo = level->GetChildByID("ufo");
		level->DestroyParticles(ufo);
		level->MakePart(ufo);
		m_loadingScene.Switch(m_scene, level);
	}
	m_scene = &m_loadingScene;
}

void LD36::Next(float score) {
	if (m_currentLevel >= m_scores.size()) {
		m_scores.resize(m_currentLevel + 1);
		if (score == -1.0f) {
			m_scores[m_currentLevel] = std::numeric_limits<float>::max();
		} else {
			m_scores[m_currentLevel] = score;
		}
	} else if (score < m_scores[m_currentLevel]) {
		m_scores[m_currentLevel] = score;
	}
	m_currentLevel++;
	Restart();
}

std::vector<float>& LD36::GetScores() {
	return m_scores;
}
