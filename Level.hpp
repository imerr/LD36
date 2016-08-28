#ifndef LD36_LEVEL_HPP
#define LD36_LEVEL_HPP
#include <Engine/Scene.hpp>
#include <SFML/Audio/Sound.hpp>

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
		sf::IntRect previewArea;
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
	bool m_levelDone;
	bool m_levelRated;
	float m_settleTimer;
	engine::util::EventHandler<const sf::Event::KeyEvent&>* m_keyHandler;
	float m_score;
	sf::Sound* m_soundSummon;
	sf::Sound* m_scoringSound;
public:
	Level(engine::Game* game);
	~Level();

	bool SetPyramid(size_t pyramid);
	virtual bool initialize(Json::Value& root);
	void MakePart(engine::Node* ufo);
	void RemovePartRope();
	void DestroyParticles(engine::Node* node);
	static std::string GetRating(float score);
protected:
	virtual void OnUpdate(sf::Time interval);

	void Rate();

};


#endif //LD36_LEVEL_HPP
