#pragma once

class GameObject;
struct ScriptContext;

class Script
{
public:
    virtual ~Script() = default;

    virtual const char* GetName() const = 0;

    virtual void OnCreate(GameObject* owner, ScriptContext& context) {}
    virtual void OnUpdate(GameObject* owner, ScriptContext& context, float deltaTime) {}
    virtual void OnDestroy(GameObject* owner, ScriptContext& context) {}
};