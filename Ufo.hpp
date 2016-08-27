#ifndef LD36_UFO_HPP
#define LD36_UFO_HPP

#include "PyramidPart.hpp"
#include <Engine/SpriteNode.hpp>

class Ufo: public engine::SpriteNode {
public:
	Ufo(engine::Scene* scene);
	~Ufo();
	void Release();
	bool HasPart() {
		return m_pyramidPart != nullptr;
	}


protected:
	virtual void OnUpdate(sf::Time interval);

	engine::util::BaseEventHandler* m_keyListener;
	PyramidPart* m_pyramidPart;
};


#endif //LD36_UFO_HPP
