#include "ReadStartupJson.h"

#include "Log/Log.h"

#include <fstream>
#include <filesystem>

const char *startupJsonFile = "./startup.json";

void ReadStartupJson::readVariables(const json &jvalue)
{
    m_directoryPath = readRawValue(jvalue, "directory path").get<std::string>();
    m_outputImagePath = readRawValue(jvalue, "output image path").get<std::string>();


    m_useAcceptedExtensions = readRawValue(jvalue, "use accepted extesions").get<bool>();
    m_acceptedExtensions = readRawValue(jvalue, "accepted extesions").get<std::vector<std::string>>();

    m_useIgnoredExtensions = readRawValue(jvalue, "use ignored extesions").get<bool>();
    m_ignoredExtensions = readRawValue(jvalue, "ignored extesions").get<std::vector<std::string>>();

    m_useAcceptedFileNames = readRawValue(jvalue, "use accepted file names").get<bool>();
    m_acceptedFileNames = readRawValue(jvalue, "accepted file names").get<std::vector<std::string>>();

    m_useIgnoredFileNames = readRawValue(jvalue, "use ignored file names").get<bool>();
    m_ignoredFileNames = readRawValue(jvalue, "ignored file names").get<std::vector<std::string>>();

    m_useAcceptedDirectories = readRawValue(jvalue, "use accepted directories").get<bool>();
    m_acceptedDirectories = readRawValue(jvalue, "accepted directories").get<std::vector<std::string>>();

    m_useIgnoredDirectories = readRawValue(jvalue, "use ignored directories").get<bool>();
    m_ignoredDirectories = readRawValue(jvalue, "ignored directories").get<std::vector<std::string>>();


    m_displayAllReadedFiles = readRawValue(jvalue , "display all readed files").get<bool>();
    m_displayAllIgnoredExtensions = readRawValue(jvalue, "display all ignored extensions").get<bool>();

    m_useQmlImprovement = readRawValue(jvalue, "use qml improvement").get<bool>();
    m_useCppImprovement = readRawValue(jvalue, "use cpp improvement").get<bool>();

    m_graphvizDotLocation = readRawValue(jvalue, "graphviz dot.exe location").get<std::string>();

    I("all keys readed correctly");
}

ReadStartupJson::ReadStartupJson(const char *userJsonFile)
    : m_jsonFile{startupJsonFile}
{
    std::error_code ec;
    if(userJsonFile != nullptr)
    {
        if(std::filesystem::exists(userJsonFile, ec))
        {
            I("using json file '%s' selected by user", userJsonFile);
            m_jsonFile = userJsonFile;
        }
        else
        {
            W("selected by user path to json '%s' doesn't exist", userJsonFile);
            I("trying '%s' path", m_jsonFile.c_str());
        }
    }

    if(!std::filesystem::exists(m_jsonFile, ec))
    {
        E("json file '%s', doesn't exist", m_jsonFile.c_str());
        exit(1);
    }

    std::ifstream iFile(m_jsonFile);
    if(!iFile.good())
    {
        E("can't open 'startup json' file: %s", m_jsonFile.c_str());
        exit(1);
    }

    json jvalue;
    try
    {
        jvalue = json::parse(iFile);
    }
    catch(const std::exception &e)
    {
        E("parsing 'startup json' failed");
        E("file: %s", m_jsonFile.c_str());
        E("reason: %s", e.what());
        exit(1);
    }

    iFile.close();

    this->readVariables(jvalue);

    I("startup json loaded");
}

json::value_type ReadStartupJson::readRawValue(const json &jvalue, const char *name)
{
    json::value_type value;
    try {
        value = jvalue[name];
    }
    catch (const std::exception &e)
    {
        E("Reading key failed! Cannot read key '%s' from initial data", name);
        E("reason: %s", e.what());
        exit(1);
    }
    if(value.is_null())
    {
        E("Reading key failed! Cannot read key '%s' from initial data", name);
        exit(1);
    }
    return value;
}

ReadStartupJson *ReadStartupJson::getInstance(const char *userJsonFile)
{
    static ReadStartupJson rsj(userJsonFile);
    return &rsj;
}

const std::string &ReadStartupJson::getDirectoryPath() const
{
    return m_directoryPath;
}

const std::string &ReadStartupJson::getOutputImagePath() const
{
    return m_outputImagePath;
}



bool ReadStartupJson::getUseAcceptedExtensions() const
{
    return m_useAcceptedExtensions;
}

const std::vector<std::string> &ReadStartupJson::getAcceptedExtensions() const
{
    return m_acceptedExtensions;
}

bool ReadStartupJson::getUseIgnoredExtensions() const
{
    return m_useIgnoredExtensions;
}

const std::vector<std::string> &ReadStartupJson::getIngoredExtensions() const
{
    return m_ignoredExtensions;
}



bool ReadStartupJson::getUseAcceptedFileNames() const
{
    return m_useAcceptedFileNames;
}

const std::vector<std::string> &ReadStartupJson::getAcceptedFileNames() const
{
    return m_acceptedFileNames;
}

bool ReadStartupJson::getUseIgnoredFileNames() const
{
    return m_useIgnoredFileNames;
}

const std::vector<std::string> &ReadStartupJson::getIngoredFileNames() const
{
    return m_ignoredFileNames;
}



bool ReadStartupJson::getUseAcceptedDirectories() const
{
    return m_useAcceptedDirectories;
}

const std::vector<std::string> &ReadStartupJson::getAcceptedDirectories() const
{
    return m_acceptedDirectories;
}

bool ReadStartupJson::getUseIgnoredDirectories() const
{
    return m_useIgnoredDirectories;
}

const std::vector<std::string> &ReadStartupJson::getIngoredDirectories() const
{
    return m_ignoredDirectories;
}



bool ReadStartupJson::getDisplayAllReadedFiles() const
{
    return m_displayAllReadedFiles;
}

bool ReadStartupJson::getDisplayAllIgnoredExtensions() const
{
    return m_displayAllIgnoredExtensions;
}

bool ReadStartupJson::getUseQmlImprovement() const
{
    return m_useQmlImprovement;
}

bool ReadStartupJson::getUseCppImprovement() const
{
    return m_useCppImprovement;
}

const std::string ReadStartupJson::getGraphvizDotLocation() const
{
    return m_graphvizDotLocation;
}
