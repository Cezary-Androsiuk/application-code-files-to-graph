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
    void readDirectoryRecursive(
        const fs::path &directory,
        std::map<std::string, int> &skipped,
        std::vector<fs::path> &accepted);
    void processFileFromDirectory(
        const fs::path &file,
        std::map<std::string, int> &skipped,
        std::vector<fs::path> &accepted);
    void processDirectoryFromDirectory(
        const fs::path &directory,
        std::map<std::string, int> &skipped,
        std::vector<fs::path> &innerDirectories);
    void findRelationsBetweenFiles();
    void createStructureForGraph();
    void startGraphviz();

    bool hasAcceptedExtension(const fs::path &path) const;
    bool notHasIgnoredExtension(const fs::path &path) const;

    bool hasAcceptedFileName(const fs::path &path) const;
    bool notHasIgnoredFileName(const fs::path &path) const;

    bool hasAcceptedDirectory(const fs::path &path) const;
    bool notHasIgnoredDirectory(const fs::path &path) const;

private:
    const ReadStartupJson *const m_startupJson;

    fs::path m_directoryPath;
    std::vector<std::shared_ptr<File>> m_files;
};

#endif // PROGRAM_H
