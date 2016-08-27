#include "PyramidPart.hpp"
#include <Engine/Scene.hpp>
#include <iostream>

PyramidPart::PyramidPart(engine::Scene* scene) : SpriteNode(scene) {

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