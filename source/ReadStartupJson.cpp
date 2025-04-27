#include "ReadStartupJson.h"

#include "Log/Log.h"

#include <fstream>

const char *startupJsonFile = "./startup.json";

void ReadStartupJson::readVariables(const json &jvalue)
{
    m_directoryPath = readRawValue(jvalue, "directory path").get<std::string>();

    m_outputPath = readRawValue(jvalue, "output path").get<std::string>();

    m_useAcceptedExtension = readRawValue(jvalue, "use accepted extesions").get<bool>();
    m_acceptedExtensions = readRawValue(jvalue, "accepted extesions").get<std::vector<std::string>>();

    m_useIgnoredExtensions = readRawValue(jvalue, "use ignored extesions").get<bool>();
    m_ignoredExtensions = readRawValue(jvalue, "ignored extesions").get<std::vector<std::string>>();

    m_displayAllReadedFiles = readRawValue(jvalue , "display all readed files").get<bool>();
    m_displayAllIgnoredExtensions = readRawValue(jvalue, "display all ignored extensions").get<bool>();

    I("all keys readed correctly");
}

ReadStartupJson::ReadStartupJson()
{

    std::ifstream iFile(startupJsonFile);
    if(!iFile.good())
    {
        E("can't open 'startup json' file: %s", startupJsonFile);
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
        E("file: %s", startupJsonFile);
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

ReadStartupJson *ReadStartupJson::getInstance()
{
    static ReadStartupJson rsj;
    return &rsj;
}

const std::string &ReadStartupJson::getDirectoryPath() const
{
    return m_directoryPath;
}

const std::string &ReadStartupJson::getOutputPath() const
{
    return m_outputPath;
}

bool ReadStartupJson::getUseAcceptedExtensions() const
{
    return m_useAcceptedExtension;
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

bool ReadStartupJson::getDisplayAllReadedFiles() const
{
    return m_displayAllReadedFiles;
}

bool ReadStartupJson::getDisplayAllIgnoredExtensions() const
{
    return m_displayAllIgnoredExtensions;
}
