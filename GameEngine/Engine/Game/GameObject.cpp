#include "GameObject.h"

void GameObject::Init(PhysicsSystem& physys,SDL_Texture* tex,AnimationManager* animationManager,float ppm) 
{
    this->animationManager = animationManager;
    texture = tex;
    m_body = physys.CreateBox(x, y, width, height, true, 1.f, 0.2f);
    m_body->SetType(b2_staticBody);
    m_body->SetBullet(true); //More accurate collision detection at high speeds
    PPM = ppm;
}

void GameObject::RefreshAnimationSystem()
{
    if (!animationManager)
        return;

    const AnimationClip* clip = animationManager->GetClipById(animationId);

    if (!clip)
    {
        std::cout << "Animation clip not found. ID: " << animationId << std::endl;
        return;
    }

    animatedSprite.SetClip(clip);
}

void GameObject::Update(float dt,ScriptContext& context)
{
    if (!m_body)
        return;

    b2Vec2 pos = m_body->GetPosition();

    b2Vec2 velocity = m_body->GetLinearVelocity();

    velocityX = velocity.x * PPM;
    velocityY = velocity.y * PPM;

    x = pos.x * PPM;
    y = pos.y * PPM;

    rect.w = static_cast<int>(width);
    rect.h = static_cast<int>(height);

    rect.x = static_cast<int>(x - width * 0.5f);
    rect.y = static_cast<int>(y - height * 0.5f);

    if (hasAnimation) {
        animatedSprite.Update(dt);
    }
    if (script) {
		UpdateScript(dt,context);
    }
}


void GameObject::RefreshPosition()
{
    if (!m_body)
        return;

    m_body->SetLinearVelocity(b2Vec2_zero);
    m_body->SetAngularVelocity(0.0f);

    m_body->SetTransform(
        b2Vec2{ x / PPM, y / PPM },
        m_body->GetAngle()
    );

    m_body->SetAwake(true);
}

void GameObject::RefreshSize()
{
    if (!m_body)
        return;

    b2Fixture* fixture = m_body->GetFixtureList();

    while (fixture)
    {
        b2Fixture* next = fixture->GetNext();
        m_body->DestroyFixture(fixture);
        fixture = next;
    }

    b2PolygonShape boxShape;

    boxShape.SetAsBox(
        width / 2.0f / PPM,
        height / 2.0f / PPM
    );

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 1.0f;
    fixtureDef.restitution = 0.2f;
    fixtureDef.friction = 0.3f;
    fixtureDef.isSensor = false;

    m_body->CreateFixture(&fixtureDef);
    m_body->ResetMassData();
    m_body->SetAwake(true);
}

void GameObject::RefreshVelocity()
{
    if (!m_body)
        return;

    m_body->SetLinearVelocity(
        b2Vec2{ velocityX / PPM, velocityY / PPM }
    );

    m_body->SetAwake(true);
}

void GameObject::RefreshBodyType() {
    std::cout << "usePhysics = " << usePhysics << " isStatic =" << isStatic << std::endl;

    if (usePhysics) {
        if (isStatic) {
            std::cout << "kinematicBody" << std::endl;

            m_body->SetType(b2_kinematicBody);
        }
        else {
            std::cout << "dynamicBody" << std::endl;

            m_body->SetType(b2_dynamicBody);
        }
    }
    else {
        std::cout << "staticBody" << std::endl;

        m_body->SetType(b2_staticBody);
    }

}

void GameObject::RefreshSprite(AssetManager& assetManager)
{
    texture = assetManager.LoadTexture(textureKey,texturePath);
}


void GameObject::Draw(RenderQueue& rq, int z)
{
    if (!active)
        return;

    if (hasAnimation)
    {
        animatedSprite.Draw(rq, &rect, z);
        return;
    }

    rq.Add({ texture, rect, SDL_Rect{0, 0, 0, 0}, z });
}

void GameObject::SetScript(std::unique_ptr<Script> newScript, ScriptContext& context)
{
    if (script)
        script->OnDestroy(this, context);

    script = std::move(newScript);
    hasScript = script != nullptr;

    if (script)
    {
        scriptName = script->GetName();
        script->OnCreate(this, context);
    }
    else
    {
        scriptName = "";
    }
}

void GameObject::RemoveScript(ScriptContext& context)
{
    if (script)
        script->OnDestroy(this, context);

    script.reset();
    hasScript = false;
    scriptName = "";
}

void GameObject::UpdateScript(float dt, ScriptContext& context)
{
    if (script)
        script->OnUpdate(this, context, dt);
}

void GameObject::PlayAnimation(int newAnimationId)
{
    if (!animationManager)
        return;

    if (hasAnimation && animationId == newAnimationId)
    {
        animatedSprite.Play();
        return;
    }

    animationId = newAnimationId;
    hasAnimation = true;

    RefreshAnimationSystem();
    animatedSprite.Play();
}

void GameObject::StopAnimation()
{
    animatedSprite.Pause();
}

