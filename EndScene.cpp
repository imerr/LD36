#include "EndScene.hpp"
#include "Level.hpp"
#include "LD36.hpp"
#include <Engine/Factory.hpp>
#include <Engine/Game.hpp>
#include <numeric>
#include <Engine/ResourceManager.hpp>

EndScene::EndScene(engine::Game* game, std::vector<float>& scores) : Scene(game), m_scoreCount(0), m_next(1) {
	m_scoringSound = engine::ResourceManager::instance()->MakeSound("assets/sound/score.ogg");
	m_keyHandler = game->OnKeyDown.MakeHandler([game](const sf::Event::KeyEvent&e){
		if (e.code == sf::Keyboard::R) {
			static_cast<LD36*>(game)->RestartGame();
		}
	});
}

EndScene::~EndScene() {
	m_game->OnKeyDown.RemoveHandler(m_keyHandler);
}
void EndScene::OnUpdate(sf::Time interval) {
	m_next -= interval.asSeconds();
	if (m_next < 0) {
		m_next = 1;
		auto viewSize = m_game->GetWindow()->getView().getSize();
		auto& scores = static_cast<LD36*>(m_game)->GetScores();

		const float height[2] = {247, 359};
		if (m_scoreCount < scores.size()) {
			m_scoringSound->play();
			float score = scores[m_scoreCount];
			std::string rating = Level::GetRating(score);
			auto r = engine::Factory::CreateChildFromFile("assets/scripts/score_"+rating+".json", this);
			size_t count = (scores.size()/2);
			size_t element = m_scoreCount;
			if (m_scoreCount >= scores.size()/2) {
				element -= count;
				count = scores.size() - count;
			}
			count++;
			r->SetPosition((1 + element) * viewSize.x/count, height[m_scoreCount >= scores.size()/2]);
			r->SetActive(true);
			r->SetRotation(0);
			m_scoreCount++;
		} else if (m_scoreCount == scores.size()){
			m_scoringSound->play();
			float avg = std::accumulate(scores.begin(), scores.end(), 0.0f)/scores.size();
			float score = 0.0;
			// favor lower scores for total grade
			for (auto s : scores) {
				if (s <= avg) score += s;
				if (s > avg) {
					score += std::max(s*0.75f, avg);
				}
			}
			score /= scores.size();
			auto r = engine::Factory::CreateChildFromFile("assets/scripts/score_"+Level::GetRating(score)+".json", this);
			r->SetActive(true);
			r->SetPosition(600, 495);
			m_scoreCount++;
		}
	}
}

