#include "Level.hpp"
#include "PyramidPart.hpp"
#include "LD36.hpp"
#include <Engine/util/json.hpp>
#include <Engine/ParticleSystem.hpp>
#include <iostream>
#include <numeric>
#include <Engine/Game.hpp>
#include <Engine/ResourceManager.hpp>
#include <vendor/tinydir.hpp>
#include <Engine/Factory.hpp>

Level::Level(engine::Game* game): Scene(game), m_pyramid(0), m_currentPart(0), m_destroyParticleTime(1),
								  m_levelDone(false), m_levelRated(false), m_settleTimer(0) {
	m_keyHandler = game->OnKeyDown.MakeHandler([this](const sf::Event::KeyEvent& e){
		if (m_levelRated) {
			if (e.code == sf::Keyboard::R) {
				static_cast<LD36*>(m_game)->Restart(m_score);
			} else if (e.code == sf::Keyboard::Q) {
				static_cast<LD36*>(m_game)->Next(m_score);
			}
		}
	});
	m_soundSummon = engine::ResourceManager::instance()->MakeSound("assets/sound/alien_summon.ogg");
	m_soundSummon->setVolume(30);
	m_scoringSound = engine::ResourceManager::instance()->MakeSound("assets/sound/score.ogg");
}
Level::~Level() {
	m_game->OnKeyDown.RemoveHandler(m_keyHandler);
	delete m_soundSummon;
}

bool Level::initialize(Json::Value& root) {
	if (!engine::Scene::initialize(root)) {
		return false;
	}
	auto pyramids = root["pyramids"];
	tinydir_dir dir;
	std::vector<std::string> levels;
	if (tinydir_open(&dir, "levels") == 0) {
		while (dir.has_next)
		{
			tinydir_file file;
			tinydir_readfile(&dir, &file);
			std::string name = file.name;
			if (file.is_reg && name.length() > 5 && name.compare(name.length() - 5, 5, ".json") == 0)
			{
				levels.push_back("levels/"+name);
			}
			tinydir_next(&dir);
		}
	}
	tinydir_close(&dir);
	std::sort(levels.begin(), levels.end());
	for (auto level : levels) {
		Json::Value l;
		if (engine::Factory::LoadJson(level, l)) {
			pyramids.append(l);
		}
	}
	for (auto p : pyramids) {
		Pyramid pyramid;
		pyramid.texture = p.get("texture", "").asString();
		pyramid.backgroundTexture = p.get("backgroundTexture", "").asString();
		pyramid.previewArea = engine::rectFromJson<int>(p["preview"]);
		pyramid.size = engine::vector2FromJson<float>(p["size"]);
		pyramid.scale = p.get("scale", 1.0f).asFloat();
		for (auto pa : p["parts"]) {
			Pyramid::Part part;
			part.area = engine::rectFromJson<int>(pa["area"]);
			part.goal = engine::vector2FromJson<float>(pa["goal"]);
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
	if (m_levelDone) {
		bool settled = true;
		m_settleTimer += interval.asSeconds();
		for (auto part: m_parts) {
			if (part.second->GetBody()->IsAwake()) {
				settled = false;
				break;
			}
		}
		// 10s timeout in case parts are stuck and are constantly wiggling
		if (settled || m_settleTimer > 10.0f) {
			Rate();
			m_levelDone = false;
		}
	}
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
		engine::ParticleSystem* p = static_cast<engine::ParticleSystem*>(GetChildByID("destroyParticle"));
		if (!p) {
			std::cerr << "No destroyParticle set :/" << std::endl;
			return;
		}
		for (auto particle : p->GetParticles()) {
			static_cast<engine::SpriteNode*>(particle)->SetColor(sf::Color(255, 255, 255, 255 * (m_destroyParticleTime/0.8f)));
		}
		m_destroyParticleTime -= interval.asSeconds();
	}
}

bool Level::SetPyramid(size_t pyramid) {
	if (pyramid >= m_pyramids.size()) {
		return false;
	}
	m_pyramid = pyramid;
	auto p = m_pyramids[pyramid];
	auto goal = m_ui->GetChildByID("goal");
	engine::SpriteNode* goalImageShadow = new engine::SpriteNode(m_scene);
	goal->AddNode(goalImageShadow);
	const float goalUISize = 120;
	const float goalUIMargin = 20;
	float scale = std::min((goalUISize-goalUIMargin)/p.size.x, (goalUISize-goalUIMargin)/p.size.y);
	goalImageShadow->SetSize(sf::Vector2f(p.size.x * scale, p.size.y * scale));
	goalImageShadow->SetTexture(p.texture, p.previewArea.width && p.previewArea.height ? &p.previewArea : nullptr);
	goalImageShadow->setOrigin(sf::Vector2f(p.size.x * scale / 2, p.size.y * scale / 2));
	goalImageShadow->SetPosition(goalUISize/2 + 2, goalUISize/2 + 1);
	goalImageShadow->SetColor(sf::Color(100, 100, 100));

	engine::SpriteNode* goalImage = new engine::SpriteNode(m_scene);
	goal->AddNode(goalImage);
	goalImage->SetSize(sf::Vector2f(p.size.x * scale - 2, p.size.y * scale - 1));
	goalImage->SetTexture(p.texture, p.previewArea.width && p.previewArea.height ? &p.previewArea : nullptr);
	goalImage->setOrigin(sf::Vector2f(p.size.x * scale / 2, p.size.y * scale / 2));
	goalImage->SetPosition(goalUISize/2, goalUISize/2);
	if (!p.backgroundTexture.empty()) {
		engine::SpriteNode* bg = static_cast<engine::SpriteNode*>(GetChildByID("bg"));
		bg->SetTexture(p.backgroundTexture);
	}
	// destroy old parts
	for (auto part : m_parts) {
		part.second->Delete();
	}
	m_parts.clear();
	auto partContainer = GetChildByID("partContainer");
	for (auto part : p.parts) {
		engine::SpriteNode* pa = new PyramidPart(this);
		pa->SetSize(sf::Vector2f(part.area.width * p.scale, part.area.height * p.scale));
		pa->SetTexture(p.texture, &part.area);
		pa->setOrigin(sf::Vector2f(part.area.width * p.scale / 2, part.area.height * p.scale / 2));

		b2BodyDef body;
		body.userData = pa;
		body.linearDamping = 0.4;
		body.type = b2_dynamicBody;
		pa->SetBody(GetWorld()->CreateBody(&body));

		b2FixtureDef def;
		b2PolygonShape poly;
		def.density = 50;
		def.restitution = 0;
		def.friction = 0.8;
		for (auto shape : part.shapes) {
			poly.SetAsBox((shape.width * p.scale  / 2 - 1) / GetPixelMeterRatio(), (shape.height  * p.scale  / 2 - 1) / GetPixelMeterRatio(),
						  b2Vec2(shape.left / GetPixelMeterRatio() * p.scale, shape.top / GetPixelMeterRatio() * p.scale),
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
	return true;
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
	auto oldPart = m_parts[m_currentPart];
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
		m_levelDone = true;
		return;
	} else {
		if (oldPart.first) {
			DestroyParticles(ufo);
		}
	}
	m_soundSummon->play();
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
	engine::Node* ufo = nullptr;
	for (auto joint = part->GetBody()->GetJointList(); joint;) {
		ufo = static_cast<engine::Node*>(joint->other->GetUserData());
		auto j = joint->joint;
		joint = joint->next;
		m_world->DestroyJoint(j);
	}
	p.first = true;
	if (ufo) {
		MakePart(ufo);
	}
}

void Level::Rate() {
	auto pyramid = m_pyramids[m_pyramid];
	b2Vec2 center;
	std::vector<float> scores;
	auto pos = [this] (engine::Node* n) {
		b2Vec2 p = n->GetBody()->GetPosition() * GetPixelMeterRatio();
		p.x -= n->getOrigin().x;
		p.y -= n->getOrigin().y;
		return p;
	};
	for (size_t i = 0; i < pyramid.parts.size(); i++) {
		auto p = pyramid.parts[i];
		auto part = m_parts[i].second;
		if (i == 0) {
			center = pos(part);
		}
		auto goal = b2Vec2(p.goal.x * pyramid.scale, p.goal.y * pyramid.scale);
		auto actual = pos(part) - center;
		b2Vec2 score = actual - goal;
		score.x = fabsf(score.x);
		score.y = fabsf(score.y);
		std::cout << i << ": " << (score.x + score.y) << std::endl;
		scores.push_back((score.x + score.y));
	}
	float avg = std::accumulate(scores.begin(), scores.end(), 0.0f)/scores.size();
	float score = 0.0;
	// favor lower scores for total grade
	for (auto s : scores) {
		if (s <= avg) score += s;
		if (s > avg) {
			score += std::max(s*0.75f, avg);
		}
	}
	score /= scores.size();
	std::cout << score  << " avg: " << avg << std::endl;
	auto report = m_ui->GetChildByID("report");
	engine::Node* grading = nullptr;
	grading = report->GetChildByID(GetRating(score));
	m_scoringSound->play();
	report->SetActive(true);
	grading->SetActive(true);
	m_levelRated = true;
	m_score = score;
}

std::string Level::GetRating(float score) {
	if (score < 2) {
		return "a++";
	} else if (score < 4) {
		return "a+";
	} else if (score < 9) {
		return "a";
	} else if (score < 15) {
		return "b";
	} else if (score < 25) {
		return "c";
	} else if (score < 35) {
		return "d";
	} else if (score < 50) {
		return "e";
	} else {
		return "f";
	}
}
