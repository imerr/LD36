#include "PyramidPart.hpp"
#include <Engine/Scene.hpp>
#include <iostream>
#include <Engine/ResourceManager.hpp>
#include <Engine/util/Random.hpp>

PyramidPart::PyramidPart(engine::Scene* scene) : SpriteNode(scene) {
	m_contactListener = m_scene->OnContactPreSolve.MakeHandler([this](b2Contact* c, const b2Manifold* m){
		if (c->GetFixtureA()->GetBody()->GetUserData() != this && c->GetFixtureB()->GetBody()->GetUserData() != this) {
			return;
		}
		float mass = this->GetBody()->GetMass();
		float impact = m->points[0].normalImpulse;
		if (impact/mass > 0.05) {
			float vol = impact/mass / 1.5f * 100;

			if (m_impactSound->getStatus() != sf::Sound::Playing || m_impactSound->getVolume() < vol*0.8) {
				engine::util::RandomFloat<float> r(0.7, 1.5);
				m_impactSound->setPitch(r());
				m_impactSound->setVolume(vol);
				m_impactSound->play();
			}
		}
	});
	m_impactSound = engine::ResourceManager::instance()->MakeSound("assets/sound/hit.ogg");
}

PyramidPart::~PyramidPart() {
	m_scene->OnContactPreSolve.RemoveHandler(m_contactListener);
}

void PyramidPart::OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta) {
	engine::SpriteNode::OnDraw(target, states, delta);
	std::lock_guard<std::mutex> lg(m_ropeVectorMutex);
	auto rope = m_scene->GetChildByID("rope");
	rope->SetActive(true);
	for (auto r : m_ropes) {
		rope->SetPosition(r.x, r.y);
		rope->SetRotation(r.z - GetRotation());
		rope->draw(target, states, delta);
	}
	rope->SetActive(false);
}

void PyramidPart::OnUpdate(sf::Time interval) {
	std::lock_guard<std::mutex> lg(m_ropeVectorMutex);
	m_ropes.clear();
	for (auto j = m_body->GetJointList(); j; j = j->next) {
		auto p = j->joint->GetAnchorB();
		auto pB = j->joint->GetAnchorA();
		float rotation = engine::util::b2AngleDeg(p, pB);
		p = GetBody()->GetLocalPoint(p);
		sf::Vector3f r;
		r.x = p.x * m_scene->GetPixelMeterRatio() + getOrigin().x;
		r.y = p.y * m_scene->GetPixelMeterRatio() + getOrigin().y;
		r.z = rotation;
		m_ropes.push_back(r);
	}
}

