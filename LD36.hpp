#ifndef LD36_LD36_HPP
#define LD36_LD36_HPP
#include <Engine/Game.hpp>

class LD36: public engine::Game {
protected:
	size_t m_currentLevel;
public:
	LD36();
	~LD36();
	void Restart();
	void Next();
};


#endif //LD36_LD36_HPP
