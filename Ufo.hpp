#ifndef LD36_UFO_HPP
#define LD36_UFO_HPP

#include <Engine/SpriteNode.hpp>

class Ufo: public engine::SpriteNode {
public:
	Ufo(engine::Scene* scene);
	~Ufo();

protected:
	virtual void PostDraw(sf::RenderTarget& target, sf::RenderStates states, float delta);

protected:
	virtual void OnUpdate(sf::Time interval);
protected:
	engine::util::BaseEventHandler* m_keyListener;
};


#endif //LD36_UFO_HPP
