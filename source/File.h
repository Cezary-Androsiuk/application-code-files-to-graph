#ifndef FILE_H
#define FILE_H

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include <Log/Log.h>

namespace fs = std::filesystem;

class File
{
public:
    File(const fs::path &rootDirPath, const fs::path &filePath);

    bool fileContentContains(const std::vector<std::string> &phrases) const;
    void addIncludeFile(const std::shared_ptr<File> &includedFile);
    void print() const;

    const fs::path &getFilePath() const;
    std::string getFilePathRelative() const;
    std::string getFileName() const;
    std::string getStem() const;
    const std::vector<std::weak_ptr<File>> &getIncludedFiles() const;

private:
    void readFileContent();

    size_t getRequiredLinesCount() const;

private:
    const fs::path &m_rootDirPath;
    fs::path m_filePath;
    std::vector<std::string> m_fileContent;
    std::vector<std::weak_ptr<File>> m_includedFiles;
};

#endif // FILE_H
