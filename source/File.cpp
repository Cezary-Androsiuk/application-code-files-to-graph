#include "File.h"

#include <fstream>

File::File(const fs::path &rootDirPath, const fs::path &filePath)
    : m_rootDirPath{rootDirPath}
    , m_filePath{filePath}
{
    const size_t requiredLinesCount = this->getRequiredLinesCount();

    // I("requiredLinesCount %llu", requiredLinesCount);
    m_fileContent.reserve(requiredLinesCount);
    m_includedFiles.reserve(32); /// i don't see how any file could have more included than that

    this->readFileContent();
}

bool File::fileContentContains(const std::vector<std::string> &phrases) const
{
    for(const auto &line : m_fileContent)
    {
        // R("'%s'\n", line.c_str());
        for(const auto &phrase : phrases)
        {
            size_t pos = line.find(phrase);
            if(pos != std::string::npos)
            {
                R("found '%s'!\n\n", phrase.c_str());
                return true;
            }
        }
    }
    return false;
}

void File::addIncludeFile(const std::shared_ptr<File> &includedFile)
{
    m_includedFiles.push_back(includedFile);
}

void File::print() const
{
    // printf("file: %s\n" "name: %s\n",
    //        m_filePath.string().c_str(),
    //        m_filePath.filename().string().c_str());
    const std::string fileName = this->getFilePathRelative();
    printf("%s\n", fileName.c_str());
    printf("lines count: %lld\n", m_fileContent.size());
    printf("included files:\n");
    for(const auto &includedFileWeak : m_includedFiles)
    {
        if(includedFileWeak.expired())
        {
            W("can't access includedFile in '%s' file", fileName.c_str());
            continue;
        }

        std::shared_ptr<File> includedFile(includedFileWeak);

        printf("\t""%s\n", includedFile->getFilePathRelative().c_str());
    }

}

const fs::path &File::getFilePath() const
{
    return m_filePath;
}

std::string File::getFilePathRelative() const
{
    return m_filePath.lexically_relative(m_rootDirPath).string();
}

std::string File::getFileName() const
{
    return m_filePath.filename().string();
}

std::string File::getStem() const
{
    return m_filePath.stem().string();
}

const std::vector<std::weak_ptr<File> > &File::getIncludedFiles() const
{
    return m_includedFiles;
}

void File::readFileContent()
{
    std::ifstream file(m_filePath);
    if(!file.is_open())
    {
        W("failed oppening file '%s' to read content\n", m_filePath.string().c_str());
        return;
    }

    std::string line;
    while(std::getline(file, line))
    {
        m_fileContent.push_back(line);
    }

    file.close();
}

size_t File::getRequiredLinesCount() const
{
    std::ifstream ifs(m_filePath);
    if(!ifs.is_open())
    {
        W("failed oppening file '%s' to read lines count\n", m_filePath.string().c_str());
        return 64;
    }
    size_t count = std::count(
        std::istreambuf_iterator<char>(ifs),
        std::istreambuf_iterator<char>(),
        '\n'
        )+1;

    ifs.close();

    size_t i=1;
    while(i < count)
    {
        if(i > 0x1'000'000)
        {
            return count;
        }
        i *= 2;
    }
    return i;
}
