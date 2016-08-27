#include "Ufo.hpp"
#include <Engine/Game.hpp>
#include <SFML/Window/Keyboard.hpp>

Ufo::Ufo(engine::Scene* scene) : SpriteNode(scene) {
	m_keyListener = m_scene->GetGame()->OnKeyDown.MakeHandler([this](const sf::Event::KeyEvent& e){
		if (e.code == sf::Keyboard::Space) {
			this->Release();
		}
	});
}

Ufo::~Ufo() {
	m_scene->GetGame()->OnKeyDown.RemoveHandler(m_keyListener);
}

void Ufo::OnUpdate(sf::Time interval) {
	auto v = m_body->GetLinearVelocity();
	v *= 0.95;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		v.x -= 0.13;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		v.x += 0.13;
	}
	m_body->SetLinearVelocity(v);
}

void Ufo::Release() {

}
