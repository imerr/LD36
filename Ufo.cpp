#include "Ufo.hpp"
#include "Level.hpp"
#include <Engine/Game.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <Engine/Factory.hpp>
#include <Engine/ResourceManager.hpp>

Ufo::Ufo(engine::Scene* scene) : SpriteNode(scene) {
	m_keyListener = m_scene->GetGame()->OnKeyDown.MakeHandler([this](const sf::Event::KeyEvent& e){
		if (e.code == sf::Keyboard::Space) {
			static_cast<Level*>(m_scene)->RemovePartRope();
		}
	});
	m_flyingSound = engine::ResourceManager::instance()->MakeSound("assets/sound/flying.ogg");
	m_flyingSound->setLoop(true);
	m_flyingSound->play();
}

Ufo::~Ufo() {
	m_scene->GetGame()->OnKeyDown.RemoveHandler(m_keyListener);
	delete m_flyingSound;
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
	auto joint = m_body->GetJointList();
	auto rope = GetChildByID("rope1");
	if (joint) {
		auto p = joint->joint->GetAnchorA();
		auto pB = joint->joint->GetAnchorB();
		float rotation = engine::util::b2AngleDeg(p, pB);
		p = GetBody()->GetLocalPoint(p);
		rope->SetPosition(p.x * m_scene->GetPixelMeterRatio() + getOrigin().x,
						  p.y * m_scene->GetPixelMeterRatio() + getOrigin().y);
		rope->SetRotation(rotation);
		joint = joint->next;
		rope->SetActive(true);
	} else {
		rope->SetActive(false);
	}
	rope = GetChildByID("rope2");
	if (joint) {
		auto p = joint->joint->GetAnchorA();
		auto pB = joint->joint->GetAnchorB();
		float rotation = engine::util::b2AngleDeg(p, pB);
		p = GetBody()->GetLocalPoint(p);
		rope->SetPosition(p.x * m_scene->GetPixelMeterRatio() + getOrigin().x,
						  p.y * m_scene->GetPixelMeterRatio() + getOrigin().y);
		rope->SetRotation(rotation);
		rope->SetActive(true);
	} else {
		rope->SetActive(false);
	}
}

void Ufo::PostDraw(sf::RenderTarget& target, sf::RenderStates states, float delta) {

}
