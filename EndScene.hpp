#ifndef LD36_ENDSCENE_HPP
#define LD36_ENDSCENE_HPP

#include <Engine/Scene.hpp>
#include <vector>
#include <SFML/Audio/Sound.hpp>

class EndScene: public engine::Scene {
protected:
	size_t m_scoreCount;
	float m_next;
	sf::Sound* m_scoringSound;
	engine::util::EventHandler<const sf::Event::KeyEvent&>* m_keyHandler;
public:
	EndScene(engine::Game* game, std::vector<float>& scores);

	virtual ~EndScene();

protected:
	virtual void OnUpdate(sf::Time interval);

};

#endif //LD36_ENDSCENE_HPP
