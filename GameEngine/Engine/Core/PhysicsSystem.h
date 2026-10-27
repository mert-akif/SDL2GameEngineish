#pragma once

#include <Box2D/Box2D.h>

class PhysicsSystem
{
public:
	bool Init(float ppm, float gravityX=0.0f, float gravityY = 0.0f);
	void Shutdown();
	void Step(float deltaTime);

	b2Body* CreateBox(float px,float py,float w,float h,bool dynamic,float density=1.f,float restution=0.5f);
	b2Body* CreateCircle(float px, float py, float radius, bool dynamic, float density = 1.f, float restution = 0.5f);
private:
	float m_pixelsPerMeter=100;
	b2World* m_world = nullptr;
};