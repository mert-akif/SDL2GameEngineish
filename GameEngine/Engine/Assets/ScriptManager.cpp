#include "ScriptManager.h"

#include <fstream>
#include <filesystem>
#include <cctype>

std::string ScriptGenerator::SanitizeClassName(const std::string& name)
{
    std::string result;

    bool makeUpper = true;

    for (char c : name)
    {
        if (std::isalnum(static_cast<unsigned char>(c)))
        {
            if (makeUpper)
            {
                result += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                makeUpper = false;
            }
            else
            {
                result += c;
            }
        }
        else
        {
            makeUpper = true;
        }
    }

    if (result.empty())
    {
        result = "NewScript";
    }

    if (std::isdigit(static_cast<unsigned char>(result[0])))
    {
        result = "Script" + result;
    }

    return result;
}

bool ScriptGenerator::CreateScriptFile(
    const std::string& scriptName,
    const std::string& folderPath,
    const std::string& registryPath
)
{
    std::string className = SanitizeClassName(scriptName);

    std::filesystem::create_directories(folderPath);

    std::string filePath = folderPath + "/" + className + ".h";

    if (std::filesystem::exists(filePath))
    {
        return false;
    }

    std::ofstream file(filePath);

    if (!file.is_open())
    {
        return false;
    }

    file << "#pragma once\n\n";
    file << "#include \"../Engine/Game/Script.h\"\n";
    file << "#include \"../Engine/Game/GameObject.h\"\n\n";


    file << "class " << className << " : public Script\n";
    file << "{\n";
    file << "public:\n";
    file << "    const char* GetName() const override\n";
    file << "    {\n";
    file << "        return \"" << className << "\";\n";
    file << "    }\n\n";

    file << "    void OnCreate(GameObject* owner) override\n";
    file << "    {\n";
    file << "        // Called when script is attached\n";
    file << "    }\n\n";

    file << "    void OnUpdate(GameObject* owner, float deltaTime) override\n";
    file << "    {\n";
    file << "        if (!owner)\n";
    file << "            return;\n\n";
    file << "        // Write your behaviour here\n";
    file << "    }\n\n";

    file << "    void OnDestroy(GameObject* owner) override\n";
    file << "    {\n";
    file << "        // Called when script is removed\n";
    file << "    }\n\n";

    file << "public:\n";
    file << "    float speed = 100.0f;\n";
    file << "};\n";

    file.close();

    return AppendToRegistry(className, registryPath);
}

bool ScriptGenerator::AppendToRegistry(
    const std::string& className,
    const std::string& registryPath
)
{
    std::ifstream inputFile(registryPath);

    if (!inputFile.is_open())
    {
        return false;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string content = buffer.str();

    inputFile.close();

    std::string includeLine = "#include \"../../Scripts/" + className + ".h\"";
    std::string registerLine = "        Register<" + className + ">(\"" + className + "\");";

    bool includeInserted = InsertLineBetweenMarkers(
        content,
        "// GENERATED_SCRIPT_INCLUDES_BEGIN",
        "// GENERATED_SCRIPT_INCLUDES_END",
        includeLine
    );

    bool registerInserted = InsertLineBetweenMarkers(
        content,
        "// GENERATED_SCRIPT_REGISTERS_BEGIN",
        "// GENERATED_SCRIPT_REGISTERS_END",
        registerLine
    );

    if (!includeInserted || !registerInserted)
    {
        return false;
    }

    std::ofstream outputFile(registryPath);

    if (!outputFile.is_open())
    {
        return false;
    }

    outputFile << content;
    outputFile.close();

    return true;
}

bool ScriptGenerator::InsertLineBetweenMarkers(
    std::string& content,
    const std::string& beginMarker,
    const std::string& endMarker,
    const std::string& lineToInsert
)
{
    // Zaten ekliyse tekrar ekleme
    if (content.find(lineToInsert) != std::string::npos)
    {
        return true;
    }

    size_t beginPos = content.find(beginMarker);
    size_t endPos = content.find(endMarker);

    if (beginPos == std::string::npos || endPos == std::string::npos)
    {
        return false;
    }

    if (beginPos > endPos)
    {
        return false;
    }

    size_t insertPos = endPos;

    content.insert(insertPos, lineToInsert + "\n");

    return true;
}