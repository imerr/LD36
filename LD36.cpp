#include "LD36.hpp"
#include "Level.hpp"

#include <Engine/Factory.hpp>

LD36::LD36(): Game(1024, 576) {
	m_scene = engine::Factory::create<Level>("assets/scripts/level.json", this);
}
LD36::~LD36() {

}