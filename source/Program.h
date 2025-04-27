#ifndef PROGRAM_H
#define PROGRAM_H

#include <filesystem>
#include <vector>
#include <map>
#include <memory>

#include <Log/Log.h>

#include "ReadStartupJson.h"
#include "File.h"

namespace fs = std::filesystem;

extern const char *graphSourceFile;

class Program
{
public:
    Program();

    void run();

private:
    void readDirectory();
    void findRelationsBetweenFiles();
    void createStructureForGraph();

    bool hasAcceptedExtension(const fs::path &path) const;
    bool notHasIgnoresExtension(const fs::path &path) const;

private:
    const ReadStartupJson *const m_startupJson;

    fs::path m_directoryPath;
    std::vector<std::shared_ptr<File>> m_files;
};

#endif // PROGRAM_H
