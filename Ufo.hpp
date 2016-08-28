#ifndef LD36_UFO_HPP
#define LD36_UFO_HPP

#include <Engine/SpriteNode.hpp>
#include <SFML/Audio/Sound.hpp>

class Ufo: public engine::SpriteNode {
protected:
	engine::util::BaseEventHandler* m_keyListener;
	sf::Sound* m_flyingSound;
public:
	Ufo(engine::Scene* scene);
	~Ufo();

protected:
	virtual void PostDraw(sf::RenderTarget& target, sf::RenderStates states, float delta);

protected:
	virtual void OnUpdate(sf::Time interval);
};


#endif //LD36_UFO_HPP
