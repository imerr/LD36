#include <Engine/util/json.hpp>
#include <iostream>
#include <Engine/ParticleSystem.hpp>
#include "Level.hpp"

Level::Level(engine::Game* game): Scene(game), m_pyramid(0), m_currentPart(0), m_destroyParticleTime(1) {
}
Level::~Level() {

}

bool Level::initialize(Json::Value& root) {
	if (!engine::Scene::initialize(root)) {
		return false;
	}
	auto pyramids = root["pyramids"];
	for (auto p : pyramids) {
		Pyramid pyramid;
		pyramid.texture = p.get("texture", "").asString();
		pyramid.size = engine::vector2FromJson<float>(p["size"]);
		pyramid.scale = p.get("scale", 1.0f).asFloat();
		for (auto pa : p["parts"]) {
			Pyramid::Part part;
			part.area = engine::rectFromJson<int>(pa["area"]);
			part.goal = engine::vector2FromJson<float>(pa["area"]);
			part.attach[0] = engine::vector2FromJson<float>(pa["attach"][0u]);
			part.attach[1] = engine::vector2FromJson<float>(pa["attach"][1u]);
			for (auto shape : pa["shapes"]) {
				part.shapes.push_back(engine::rectFromJson<float>(shape));
			}
			pyramid.parts.push_back(part);
		}
		m_pyramids.push_back(pyramid);
	}
	return true;
}

void Level::OnUpdate(sf::Time interval) {
	engine::Scene::OnUpdate(interval);
	if (m_destroyParticleTime < 0) {
		engine::ParticleSystem* p = static_cast<engine::ParticleSystem*>(GetChildByID("destroyParticle"));
		if (!p) {
			std::cerr << "No destroyParticle set :/" << std::endl;
			return;
		}
		p->SetActive(false);
	} else if (m_destroyParticleTime > 0.8) {
		m_destroyParticleTime -= interval.asSeconds();
	} else if (m_destroyParticleTime > 0) {
		m_destroyParticleTime -= interval.asSeconds();
		engine::ParticleSystem* p = static_cast<engine::ParticleSystem*>(GetChildByID("destroyParticle"));
		if (!p) {
			std::cerr << "No destroyParticle set :/" << std::endl;
			return;
		}
		for (auto particle : p->GetParticles()) {
			static_cast<engine::SpriteNode*>(particle)->SetColor(sf::Color(255, 255, 255, 255 * (m_destroyParticleTime/0.8f)));
		}
	}
}

void Level::SetPyramid(size_t pyramid) {
	if (pyramid >= m_pyramids.size()) {
		std::cerr << pyramid << "is not a valid pyramid type (max " << (m_pyramids.size() - 1) << ")" << std::endl;
		return;
	}
	m_pyramid = pyramid;
	auto p = m_pyramids[pyramid];
	auto goal = m_ui->GetChildByID("goal");
	engine::SpriteNode* goalImage = new engine::SpriteNode(m_scene);
	goal->AddNode(goalImage);
	const float goalUISize = 120;
	const float goalUIMargin = 15;
	float scale = std::min((goalUISize-goalUIMargin)/p.size.x, (goalUISize-goalUIMargin)/p.size.y);
	goalImage->SetSize(sf::Vector2f(p.size.x * scale, p.size.y * scale));
	goalImage->SetTexture(p.texture);
	goalImage->setOrigin(sf::Vector2f(p.size.x * scale / 2, p.size.y * scale / 2));
	goalImage->SetPosition(goalUISize/2, goalUISize/2);

	// destroy old parts
	for (auto part : m_parts) {
		part.second->Delete();
	}
	m_parts.clear();
	auto partContainer = GetChildByID("partContainer");
	for (auto part : p.parts) {
		engine::SpriteNode* pa = new engine::SpriteNode(this);
		pa->SetSize(sf::Vector2f(part.area.width * p.scale, part.area.height * p.scale));
		pa->SetTexture(p.texture, &part.area);
		pa->setOrigin(sf::Vector2f(part.area.width * p.scale / 2, part.area.height * p.scale / 2));

		b2BodyDef body;
		body.userData = pa;
		body.linearDamping = 0.2;
		body.type = b2_dynamicBody;
		pa->SetBody(GetWorld()->CreateBody(&body));

		b2FixtureDef def;
		b2PolygonShape poly;
		def.density = 3;
		def.friction = 0.4;
		for (auto shape : part.shapes) {
			poly.SetAsBox(shape.width / GetPixelMeterRatio() * scale, shape.height / GetPixelMeterRatio() * scale,
						  b2Vec2(shape.left / GetPixelMeterRatio() * scale, shape.top / GetPixelMeterRatio() * scale),
						  0);
			def.shape = &poly;
			pa->GetBody()->CreateFixture(&def);
		}
		partContainer->AddNode(pa);
		pa->SetActive(false);

		m_parts.push_back(std::make_pair(false, pa));
	}
	// Make it roll over to the first part on the first MakePart call
	m_currentPart = m_parts.size() - 1;
}

void Level::DestroyParticles(engine::Node* node) {
	engine::ParticleSystem* p = static_cast<engine::ParticleSystem*>(GetChildByID("destroyParticle"));
	if (!p) {
		std::cerr << "No destroyParticle set :/" << std::endl;
		return;
	}
	p->Reset();
	p->SetPosition(node->GetPosition());
	p->SetEmitterSize(sf::Vector2f(node->GetSize().x * 0.8f, node->GetSize().y / 0.8f));
	p->SetActive(true);
	for (auto particle : p->GetParticles()) {
		static_cast<engine::SpriteNode*>(particle)->SetColor(sf::Color::White);
	}
	m_destroyParticleTime = 3.0f;

}

void Level::MakePart(engine::Node* ufo) {
	engine::Node* part = nullptr;
	for (size_t cur = (m_currentPart + 1) % m_parts.size(); m_currentPart != cur; cur = (cur + 1) % m_parts.size()) {
		auto& p = m_parts[cur];
		if (!p.first) {
			part = p.second;
			m_currentPart = cur;
			break;
		}
	}
	if (!part) {
		// Yay, no more parts
		// Probably do some judging or something
		return;
	}
	part->SetActive(true);
	part->GetBody()->SetLinearVelocity(b2Vec2(0,0));
	part->SetPosition(ufo->GetPosition());
	auto pb = part->GetBody();
	auto ub = ufo->GetBody();
	b2RopeJointDef r;
	r.bodyA = ub;
	r.bodyB = pb;
	r.collideConnected = false;
	r.maxLength = 150 / GetPixelMeterRatio();
	sf::Vector2f ufoAttach[2] {{25, 18}, {-25, 18}};
	auto currentPyramid = m_pyramids[m_pyramid];
	auto partInfo = currentPyramid.parts[m_currentPart];
	for (size_t i = 0; i < 2; i++) {
		r.localAnchorA = b2Vec2(ufoAttach[i].x / GetPixelMeterRatio(), ufoAttach[i].y / GetPixelMeterRatio());
		r.localAnchorB = b2Vec2(partInfo.attach[i].x / GetPixelMeterRatio() * currentPyramid.scale,
								partInfo.attach[i].y / GetPixelMeterRatio() * currentPyramid.scale);
		m_world->CreateJoint(&r);
	}

}

void Level::RemovePartRope() {
	auto& p = m_parts[m_currentPart];
	engine::Node* part = p.second;
	auto joint = part->GetBody()->GetJointList();
	if (joint) {
		if (!joint->next) {
			auto ufo = static_cast<engine::Node*>(joint->other->GetUserData());
			DestroyParticles(ufo);
			MakePart(ufo);
			p.first = true;
		}
		m_world->DestroyJoint(joint->joint);
	}
}
