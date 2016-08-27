#ifndef LD36_LEVEL_HPP
#define LD36_LEVEL_HPP
#include <Engine/Scene.hpp>

class Level: public engine::Scene {
public:
	struct Pyramid {
		struct Part {
			sf::IntRect area;
			sf::Vector2f goal;
			sf::Vector2f attach[2];
			std::vector<sf::FloatRect> shapes;
		};
		std::string texture;
		sf::Vector2f size;
		float scale;
		std::vector<Part> parts;
	};
protected:
	std::vector<Pyramid> m_pyramids;
	size_t m_pyramid;
	std::vector<std::pair<bool, engine::Node*>> m_parts;
	size_t m_currentPart;
	float m_destroyParticleTime;
public:
	Level(engine::Game* game);
	~Level();

	void SetPyramid(size_t pyramid);
	virtual bool initialize(Json::Value& root);
	void MakePart(engine::Node* ufo);
	void RemovePartRope();
	void DestroyParticles(engine::Node* node);
protected:
	virtual void OnUpdate(sf::Time interval);
};


#endif //LD36_LEVEL_HPP
