#include "Program.h"

const char *graphSourceFile = "./graph_source.dot";

Program::Program()
    : m_startupJson( ReadStartupJson::getInstance() )
{

}

void Program::run()
{
    m_directoryPath = m_startupJson->getDirectoryPath();

    this->readDirectory();

    this->findRelationsBetweenFiles();

    // for(const auto &file : m_files)
    // {
    //     file->print();
    //     printf("\n");
    // }

    this->createStructureForGraph();

    this->startGraphviz();

}

void Program::readDirectory()
{
    std::map<std::string, int> skipped;
    std::vector<fs::path> accepted;

    this->readDirectoryRecursive(m_directoryPath, skipped, accepted);

    if(m_startupJson->getDisplayAllIgnoredExtensions())
    {
        R("Skipped files:\n");
        for(const auto &[skipped, count] : skipped)
        {
            R("\"%s\" - %d\n", skipped.c_str(), count);
        }
    }

    if(m_startupJson->getDisplayAllReadedFiles())
    {
        R("Accepted files:\n");
        for(const auto &file : accepted)
        {
            R("%s\n", file.relative_path().string().c_str());
        }
    }
}

void Program::readDirectoryRecursive(
    const fs::path &directory,
    std::map<std::string, int> &skipped,
    std::vector<fs::path> &accepted)
{
    std::vector<fs::path> innerDirectories;

    for(const auto &file : fs::directory_iterator(directory))
    {
        if(file.is_directory())
        {
            this->processDirectoryFromDirectory(file.path(), skipped, innerDirectories);
            continue;
        }
        if(!file.is_regular_file())
            continue;

        this->processFileFromDirectory(file, skipped, accepted);
    }

    /// read inned directories
    for(const auto &directory : innerDirectories)
    {
        this->readDirectoryRecursive(directory, skipped, accepted);
    }
}

void Program::processFileFromDirectory(
    const fs::path &file,
    std::map<std::string, int> &skipped,
    std::vector<fs::path> &accepted)
{
    bool goodExtension = this->hasAcceptedExtension(file) &&
                         this->notHasIgnoredExtension(file);

    bool goodFileName = this->hasAcceptedFileName(file) &&
                        this->notHasIgnoredFileName(file);

    if(goodExtension && goodFileName)
    {
        accepted.push_back(file.lexically_relative(m_directoryPath));

        m_files.push_back( std::make_shared<File>(m_directoryPath, file) );
    }
    else
    {
        std::string skippedData;

        /// if filename is not an "unwanted" one, then it must be an extension at this point
        /// and if both are "unwanted" imo filename has higher privileges
        if(!goodFileName)
        {
            /// file name is marked as "unwanted"
            skippedData = file.lexically_relative(m_directoryPath).string();
        }
        else // if(!goodExtension)
        {
            skippedData = "*" + file.extension().string();
        }

        /// add to skipped data list
        if(auto search = skipped.find(skippedData); search != skipped.end())
        {
            search->second++;
        }
        else
        {
            skipped[skippedData] = 1;
        }
    }
}

void Program::processDirectoryFromDirectory(
    const fs::path &directory,
    std::map<std::string, int> &skipped,
    std::vector<fs::path> &innerDirectories)
{
    if(this->hasAcceptedDirectory(directory) && this->notHasIgnoredDirectory(directory))
    {
        innerDirectories.push_back(directory);
    }
    else
    {
        std::string skippedData = directory.lexically_relative(m_directoryPath)
                                      .string() + "/";

        /// add to skipped data list
        if(auto search = skipped.find(skippedData); search != skipped.end())
        {
            search->second++;
        }
        else
        {
            skipped[skippedData] = 1;
        }
    }

}

void Program::findRelationsBetweenFiles()
{
    // int filesToProcess = m_files.size();
    // int fileIndexProcessing = 0;
    // printf("looking for realtions... %3d/%3d  ", fileIndexProcessing++, filesToProcess);
    // fflush(stdout);
    for(const auto &fileWithContent : m_files)
    {
        for(const auto &fileWithName : m_files)
        {
            if(fileWithContent.get() == fileWithName.get())
                continue;


            std::vector<std::string> phrases;
            if(ReadStartupJson::getInstance()->getUseQmlImprovement())
            {
                phrases.push_back(fileWithName->getFileName());
                auto stem = fileWithName->getStem();
                phrases.push_back(stem + "{");
                phrases.push_back(stem + " {");
            }
            else if(ReadStartupJson::getInstance()->getUseCppImprovement())
            {
                auto filename = fileWithName->getFileName();

                phrases.push_back("\"" + filename + "\"");
                phrases.push_back("\\" + filename + "\"");
                phrases.push_back("/" + filename + "\"");

                phrases.push_back("<" + filename + ">");
                phrases.push_back("\\" + filename + ">");
                phrases.push_back("/" + filename + ">");
            }
            else
            {
                phrases.push_back(fileWithName->getFileName());
            }

            // R("START ####################\n");
            // R("in file: %s\n", fileWithContent->getFileName().c_str());
            // for(const auto &phrase : phrases)
            //     R("phrase: %s\n", phrase.c_str());

            if(fileWithContent->fileContentContains(phrases))
            {
                // R("Found!\n");
                fileWithContent->addIncludeFile(fileWithName);
            }
        }
        // printf("\r""looking for realtions... %3d/%3d  ", fileIndexProcessing++, filesToProcess);
        // fflush(stdout);
    }

    I("created relations between files");
    // printf("\n");
}

void Program::createStructureForGraph()
{
    std::ofstream outFile(graphSourceFile);
    if(!outFile.is_open())
    {
        E("failed oppening file '%s' to save structure for graph\n", graphSourceFile);
        return;
    }

    outFile << "digraph G {\n";

    for(const auto &file : m_files)
    {
        const std::string fileName = file->getFileName();
        for(const auto &includedFileWeak : file->getIncludedFiles())
        {
            if(includedFileWeak.expired())
            {
                W("can't access includedFile in '%s' file", fileName.c_str());
                continue;
            }

            std::shared_ptr<File> includedFile(includedFileWeak);

            outFile << "\""
                    << includedFile->getFileName()
                    << "\" -> \""
                    << fileName
                    << "\"" << ";\n";
        }
    }
    outFile << "}";

    outFile.close();
}

void Program::startGraphviz()
{
    std::string graphvizDotLocation = m_startupJson->getGraphvizDotLocation();
    std::string outImagePath = m_startupJson->getOutputImagePath();



    auto makePathSafe = [](std::string path) -> std::string {
        if (path.find(' ') != std::string::npos) {
            return "\"" + path + "\"";
        }
        return path;
    };

    auto makeWindowsLikePath = [&makePathSafe](std::string path) -> std::string {
        /// change normal slash '/' to stupid windows slash '\'
        for (char& c : path) {
            if (c == '/') c = '\\';
        }

        return makePathSafe(path);
    };

#ifdef _WIN32
    std::string command =
        makeWindowsLikePath(graphvizDotLocation) +
        " -Tpng " +
        makeWindowsLikePath(graphSourceFile) +
        " -o " +
        makeWindowsLikePath(outImagePath);

// #elif __linux__
#else
    std::string command =
        makePathSafe(graphvizDotLocation) +
        " -Tpng " +
        makePathSafe(graphSourceFile) +
        " -o " +
        makePathSafe(outImagePath);
#endif


    if(std::system(command.c_str()))
    {
        E("failed while starting graphviz with command: \"%s\"", command.c_str());
    }
    else
    {
        I("graphviz executed successfully");
    }

    // fs::remove(graphSourceFile);
}




bool Program::hasAcceptedExtension(const std::filesystem::__cxx11::path &path) const
{
    static bool useAcceptedExtensions = m_startupJson->getUseAcceptedExtensions();
    if(!useAcceptedExtensions)
        return true;

    static const auto &acceptedExtensions = m_startupJson->getAcceptedExtensions();
    const std::string &extension = path.extension().string();

    for(const std::string &acceptedExtension : acceptedExtensions)
    {
        if(acceptedExtension.empty())
            continue;

        if(acceptedExtension[0] == '.')
        {
            if(extension == acceptedExtension)
                return true;
        }
        else
        {
            if(extension == '.' + acceptedExtension)
                return true;
        }
    }
    return false;
}

bool Program::notHasIgnoredExtension(const std::filesystem::__cxx11::path &path) const
{
    static bool useIgnoredExtensions = m_startupJson->getUseIgnoredExtensions();
    if(!useIgnoredExtensions)
        return true;

    static const auto &ignoredExtensions = m_startupJson->getIngoredExtensions();
    const std::string &extension = path.extension().string();

    for(const std::string &ignoredExtension : ignoredExtensions)
    {
        if(ignoredExtension.empty())
            continue;

        if(ignoredExtension[0] == '.')
        {
            if(extension == ignoredExtension)
                return false;
        }
        else
        {
            if(extension == '.' + ignoredExtension)
                return false;
        }
    }
    return true;
}

bool Program::hasAcceptedFileName(const std::filesystem::__cxx11::path &path) const
{
    static bool useAcceptedFileNames = m_startupJson->getUseAcceptedFileNames();
    if(!useAcceptedFileNames)
        return true;

    static const auto &acceptedFileNames = m_startupJson->getAcceptedFileNames();
    const std::string &fileName = path.filename().string();

    for(const std::string &acceptedFileName : acceptedFileNames)
    {
        if(acceptedFileName.empty())
            continue;

        if(fileName == acceptedFileName)
            return true;
    }
    return false;
}

bool Program::notHasIgnoredFileName(const std::filesystem::__cxx11::path &path) const
{
    static bool useIgnoredFileNames = m_startupJson->getUseIgnoredFileNames();
    if(!useIgnoredFileNames)
        return true;

    static const auto &ignoredFileNames = m_startupJson->getIngoredFileNames();
    const std::string &fileName = path.filename().string();

    for(const std::string &ignoredFileName : ignoredFileNames)
    {
        if(ignoredFileName.empty())
            continue;

        if(fileName == ignoredFileName)
            return false;
    }
    return true;
}

bool Program::hasAcceptedDirectory(const std::filesystem::__cxx11::path &path) const
{
    static bool useAcceptedDirectories = m_startupJson->getUseAcceptedDirectories();
    if(!useAcceptedDirectories)
        return true;

    static const auto &acceptedDirectories = m_startupJson->getAcceptedDirectories();
    const std::string &directory = path.lexically_relative(m_directoryPath).string();

    for(const std::string &acceptedDirectory : acceptedDirectories)
    {
        if(acceptedDirectory.empty())
            continue;

        char last = acceptedDirectory[acceptedDirectory.size()-1];
        if(last == '/')
        {
            if((directory + "/") == acceptedDirectory)
                return true;
        }
        else if(last == '\\')
        {
            if((directory + "\\") == acceptedDirectory)
                return true;
        }
        else
        {
            if(directory == acceptedDirectory)
                return true;
        }
    }
    return false;
}

bool Program::notHasIgnoredDirectory(const std::filesystem::__cxx11::path &path) const
{
    static bool useIgnoredDirectories = m_startupJson->getUseIgnoredDirectories();
    if(!useIgnoredDirectories)
        return true;

    static const auto &ignoredDirectories = m_startupJson->getIngoredDirectories();
    const std::string &directory = path.filename().string();

    for(const std::string &ignoredDirectory : ignoredDirectories)
    {
        if(ignoredDirectory.empty())
            continue;

        char last = ignoredDirectory[ignoredDirectory.size()-1];
        if(last == '/')
        {
            if((directory + "/") == ignoredDirectory)
                return false;
        }
        else if(last == '\\')
        {
            if((directory + "\\") == ignoredDirectory)
                return false;
        }
        else
        {
            if(directory == ignoredDirectory)
                return false;
        }
    }
    return true;
}

