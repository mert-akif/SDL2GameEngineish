#include "PhysicsSystem.h"

bool PhysicsSystem::Init(float ppm, float gravityX, float gravityY)
{
	m_pixelsPerMeter = ppm;
	m_world = new b2World(b2Vec2(gravityX, gravityY));
	return true;
}

void PhysicsSystem::Shutdown()
{
	delete m_world;
	m_world = nullptr;
}

void PhysicsSystem::Step(float deltaTime)
{
	if (m_world) {
		m_world->Step(deltaTime, 8, 3);
	}
}

b2Body* PhysicsSystem::CreateBox(float px, float py, float w, float h, bool dynamic, float density, float restitution)
{
	if (!m_world)
		return nullptr;

	b2BodyDef bodyDef;
	bodyDef.type = dynamic ? b2_dynamicBody : b2_staticBody;
	bodyDef.position.Set(px / m_pixelsPerMeter, py / m_pixelsPerMeter);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(
		w / 2.0f / m_pixelsPerMeter,
		h / 2.0f / m_pixelsPerMeter
	);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &boxShape;
	fixtureDef.density = density;
	fixtureDef.restitution = restitution;
	fixtureDef.friction = 0.1f;
	fixtureDef.isSensor = false;

	b2Body* body = m_world->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);

	return body;
}

b2Body * PhysicsSystem::CreateCircle(float px, float py,float radius, bool dynamic, float density, float restitution)
{
	if (!m_world) return nullptr;
	b2BodyDef bodyDef;
	bodyDef.type = dynamic ? b2_dynamicBody : b2_staticBody;
	bodyDef.position.Set(px / m_pixelsPerMeter, py / m_pixelsPerMeter);

	b2CircleShape circleShape;
	circleShape.m_radius = radius / 2 / m_pixelsPerMeter;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circleShape;
	fixtureDef.density = density;
	fixtureDef.restitution = restitution;

	b2Body* body = m_world->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
	return body;
}