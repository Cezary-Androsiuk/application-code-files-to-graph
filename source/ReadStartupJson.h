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
    const std::string &getOutputImagePath() const;


    bool getUseAcceptedExtensions() const;
    const std::vector<std::string> &getAcceptedExtensions() const;

    bool getUseIgnoredExtensions() const;
    const std::vector<std::string> &getIngoredExtensions() const;

    bool getUseAcceptedFileNames() const;
    const std::vector<std::string> &getAcceptedFileNames() const;

    bool getUseIgnoredFileNames() const;
    const std::vector<std::string> &getIngoredFileNames() const;

    bool getUseAcceptedDirectories() const;
    const std::vector<std::string> &getAcceptedDirectories() const;

    bool getUseIgnoredDirectories() const;
    const std::vector<std::string> &getIngoredDirectories() const;


    bool getDisplayAllReadedFiles() const;
    bool getDisplayAllIgnoredExtensions() const;

    bool getUseQmlImprovement() const;
    bool getUseCppImprovement() const;

    const std::string getGraphvizDotLocation() const;

private:
    std::string m_directoryPath;
    std::string m_outputImagePath;


    bool m_useAcceptedExtensions;
    std::vector<std::string> m_acceptedExtensions;

    bool m_useIgnoredExtensions;
    std::vector<std::string> m_ignoredExtensions;

    bool m_useAcceptedFileNames;
    std::vector<std::string> m_acceptedFileNames;

    bool m_useIgnoredFileNames;
    std::vector<std::string> m_ignoredFileNames;

    bool m_useAcceptedDirectories;
    std::vector<std::string> m_acceptedDirectories;

    bool m_useIgnoredDirectories;
    std::vector<std::string> m_ignoredDirectories;


    bool m_displayAllReadedFiles;
    bool m_displayAllIgnoredExtensions;

    bool m_useQmlImprovement;
    bool m_useCppImprovement;

    std::string m_graphvizDotLocation;

};

#endif // READSTARTUPJSON_H
