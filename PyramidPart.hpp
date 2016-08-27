#ifndef LD36_PYRAMIDPART_HPP
#define LD36_PYRAMIDPART_HPP


#include <Engine/SpriteNode.hpp>
#include <thread>
#include <mutex>

class PyramidPart: public engine::SpriteNode {
protected:
	std::vector<sf::Vector3f> m_ropes;
	std::mutex m_ropeVectorMutex;
public:
	PyramidPart(engine::Scene* scene);

protected:
	virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta);

	virtual void OnUpdate(sf::Time interval);
};


#endif //LD36_PYRAMIDPART_HPP
