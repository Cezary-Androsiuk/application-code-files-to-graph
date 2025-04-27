#ifndef READSTARTUPJSON_H
#define READSTARTUPJSON_H

#include "nlohmann_json/json.hpp"

#include <string>
#include <vector>

using json = nlohmann::json;

extern const char *startupJsonFile;

class ReadStartupJson
{
    void readVariables(const json &jvalue);

    ReadStartupJson();

    json::value_type readRawValue(const json &jvalue, const char *name);

public:
    static ReadStartupJson *getInstance();


    const std::string &getDirectoryPath() const;

    const std::string &getOutputPath() const;

    bool getUseAcceptedExtensions() const;
    const std::vector<std::string> &getAcceptedExtensions() const;

    bool getUseIgnoredExtensions() const;
    const std::vector<std::string> &getIngoredExtensions() const;

    bool getDisplayAllReadedFiles() const;
    bool getDisplayAllIgnoredExtensions() const;

private:
    std::string m_directoryPath;
    std::string m_outputPath;
    bool m_useAcceptedExtension;
    std::vector<std::string> m_acceptedExtensions;
    bool m_useIgnoredExtensions;
    std::vector<std::string> m_ignoredExtensions;
    bool m_displayAllReadedFiles;
    bool m_displayAllIgnoredExtensions;
};

#endif // READSTARTUPJSON_H
