#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "../Game/Script.h"


// GENERATED_SCRIPT_INCLUDES_BEGIN

#include "../../Scripts/PlayerMovementsCreated.h"
// GENERATED_SCRIPT_INCLUDES_END

class ScriptRegistry
{
public:
    struct ScriptEntry
    {
        std::string name;
        std::function<std::unique_ptr<Script>()> createFunction;
    };

public:
    ScriptRegistry()
    {
        RegisterScripts();
    }

    void RegisterScripts()
    {
        m_scripts.clear();


        // GENERATED_SCRIPT_REGISTERS_BEGIN
                Register<PlayerMovementsCreated>("PlayerControllerScriptCreated");
        // GENERATED_SCRIPT_REGISTERS_END
    }

    template<typename T>
    void Register(const std::string& name)
    {
        m_scripts.push_back({
            name,
            []()
            {
                return std::make_unique<T>();
            }
            });
    }

    const std::vector<ScriptEntry>& GetScripts() const
    {
        return m_scripts;
    } 

    std::unique_ptr<Script> CreateScript(const std::string& name)
    {
        for (const ScriptEntry& entry : m_scripts)
        {
            if (entry.name == name)
            {
                return entry.createFunction();
            }
        }

        return nullptr;
    }

private:
    std::vector<ScriptEntry> m_scripts;
};

class ScriptGenerator
{
public:
    static bool CreateScriptFile(
        const std::string& scriptName,
        const std::string& folderPath,
        const std::string& registryPath
    );

private:
    static std::string SanitizeClassName(const std::string& name);

    static bool AppendToRegistry(
        const std::string& className,
        const std::string& registryPath
    );

    static bool InsertLineBetweenMarkers(
        std::string& content,
        const std::string& beginMarker,
        const std::string& endMarker,
        const std::string& lineToInsert
    );
};