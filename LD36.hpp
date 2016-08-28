#ifndef LD36_LD36_HPP
#define LD36_LD36_HPP

#include <Engine/Game.hpp>
#include <vector>

class LD36: public engine::Game {
protected:
	size_t m_currentLevel;
	std::vector<float> m_scores;
public:
	LD36();
	~LD36();
	void Restart(float score = -1.0f);
	void Next(float score = -1.0f);
	std::vector<float>& GetScores();

	void RestartGame();
protected:
	void RegisterScore(float score);
};


#endif //LD36_LD36_HPP
