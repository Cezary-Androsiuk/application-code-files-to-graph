#include "Log.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>
#include <cstring>

const char *outputDirectory = "logs/";

bool Log::firstLog = true;

// LogSession Log::currentSession = LogSession();
std::string Log::currentSession = std::string();
std::string Log::fileName;
std::ofstream Log::outFile;

const char *Log::logTypeToStr(Type type)
{
    switch (type) {
    case Log::Type::Info:       return "Log::Type::Info";
    case Log::Type::Warning:    return "Log::Type::Warning";
    case Log::Type::Error:      return "Log::Type::Error";
    case Log::Type::Debug:      return "Log::Type::Debug";
    case Log::Type::Raw:        return "Log::Type::Raw";
    }

    fprintf(stderr, "unknown Log::Type: %d\n", static_cast<int>(type));
    fflush(stderr);

    return "<unknown Log::Type>";
}

const char *Log::logActionToStr(Action action)
{
    switch (action) {
    case Log::Action::None:             return "Log::Action::None";
    case Log::Action::Save:             return "Log::Action::Save";
    case Log::Action::Print:            return "Log::Action::Print";
    case Log::Action::Session:          return "Log::Action::Session";
    case Log::Action::SavePrint:        return "Log::Action::SavePrint";
    case Log::Action::SaveSession:      return "Log::Action::SaveSession";
    case Log::Action::PrintSession:     return "Log::Action::PrintSession";
    case Log::Action::All:              return "Log::Action::All";
    }

    fprintf(stderr, "unknown Log::Action: %d\n", static_cast<int>(action));
    fflush(stderr);

    return "<unknown Log::Action>";
}

void Log::info(cstr func, cstr log, Log::Action action)
{
    Log::safeLog(Type::Info, func, log, action);
}

void Log::warning(cstr func, cstr log, Log::Action action)
{
    Log::safeLog(Type::Warning, func, log, action);
}

void Log::error(cstr func, cstr log, Log::Action action)
{
    Log::safeLog(Type::Error, func, log, action);
}

void Log::debug(cstr func, cstr log, Log::Action action)
{
    Log::safeLog(Type::Debug, func, log, action);
}

void Log::raw(cstr func, cstr log, Action action)
{
    Log::safeLog(Type::Raw, func, log, action);
}

std::string Log::asprintf(const char *text, ...)
{
    va_list args;
    va_start(args, text);

    va_list argsCopy;
    va_copy(argsCopy, args);
    const int size = vsnprintf(nullptr, 0, text, argsCopy);
    va_end(argsCopy);

    if (size <= 0) {
        va_end(args);
        return "";
    }


    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size+1);
    vsnprintf(buffer.get(), size+1, text, args);
    va_end(args);

    return std::string(buffer.get(), size);
}

std::string Log::asprintf(cstr text, ...)
{
    va_list args;
    va_start(args, text.c_str());

    std::string str = Log::asprintf(text.c_str(), args);

    va_end(args);

    return str;
}


std::string Log::time(bool simpleSeparators)
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) % 1000;

    std::tm tm = *std::localtime(&now_time_t);

    std::ostringstream oss;
    if(simpleSeparators)
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S_")
            << std::setfill('0') << std::setw(3) << now_ms.count();
    else
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S.")
            << std::setfill('0') << std::setw(3) << now_ms.count();

    return oss.str();
}

std::string Log::buildPrefix(Log::Type logType, cstr funName)
{
    std::string prefix;

    // set type
    switch (logType) {
    case Log::Type::Info:       prefix += "I ";         break;
    case Log::Type::Warning:    prefix += "W ###";      break;
    case Log::Type::Error:      prefix += "E ### ###";  break;
    case Log::Type::Debug:      prefix += "D ";         break;
    case Log::Type::Raw:        prefix += "R ";         break;
    default:
        fprintf(stderr, "unknown type %s, using: '?'\n", Log::logTypeToStr(logType));
        fflush(stderr);
        prefix += "? ";
        break;
    }

    // set function name
    if(funName.length() >= EST_FUNCTION_LENGTH)
        prefix += funName;
    else
    {
        size_t fill = EST_FUNCTION_LENGTH - funName.length() - prefix.size();
        prefix += std::string(fill, SHORTER_FUNCTION_FILL_CHARACTER);
        prefix += funName;
    }

#if SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT
    prefix += " ";
#endif

    // set space between content
    prefix += std::string(CONTENT_SPACE, CONTENT_SPACE_CHARACTER);

#if SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT
    prefix += " ";
#endif

    return prefix;
}

std::string Log::buildStartPrefix()
{
    static const char *startText = "--- [APPLICATION STARTED] ---";
    static const std::string spaceText =
        std::string(EST_FUNCTION_LENGTH -8 -28 +3, '-');

    std::string prefix;
    prefix = /*"\n\n"*/ "[" + Log::time() +  "]";

    return prefix + spaceText + startText + spaceText;
}

void Log::log(Log::Type logType, cstr funName, cstr log, Log::Action action)
{
    Action limitedAction = Action( (action | Log::actionForceLowest) & Log::actionForceHighest );

    const std::string time = "[" + Log::time() +  "]" + " ";
    const std::string prefix = buildPrefix(logType, funName);

    bool isRaw = logType == Log::Type::Raw;
    if(isRaw)
    {
        if(limitedAction & Action::Print)
            Log::print(log, false);
    }
    else
    {
        if(limitedAction & Action::Print)
            Log::print(prefix + log, true);
    }

    if(isRaw)
    {
        if(limitedAction & Action::Save)
            Log::saveFile(time + prefix + "\n""<<START RAW>>""\n" + log + "\n""<<END RAW>>");
    }
    else
    {
        if(limitedAction & Action::Save)
            Log::saveFile(time + prefix + log);
    }

    try{
        if(isRaw)
        {
            if(limitedAction & Action::Session)
            {
                Log::addSession(log, false);
                // Log::addSession(logType, funName, log);
            }
        }
        else
        {
            if(limitedAction & Action::Session)
            {
                Log::addSession(prefix + log, true);
                // Log::addSession(logType, funName, log);
            }
        }
    }
    catch (...) {
        fprintf(stderr, "adding log to session failed\n");
        fflush(stderr);
    }

    if(firstLog)
        firstLog = false;
}

void Log::safeLog(Log::Type logType, cstr funName, cstr log, Action action)
{
    try {
        Log::log(logType, funName, log, action);
    } catch (...) {
        fprintf(stderr, "logging failed\n");
        fflush(stderr);
    }
}

void Log::print(cstr content, bool newLine)
{
    fprintf(stdout, "%s%s", content.c_str(), newLine ? "\n" : "");
    fflush(stdout);
}

void Log::saveFile(cstr content)
{
    if(!outFile.is_open())
    {
        if(!std::filesystem::exists(outputDirectory))
        {
            if(!std::filesystem::create_directory(outputDirectory))
            {
                fprintf(stderr, "cannot create '%s' output directory\n", outputDirectory);
                fflush(stderr);
                return;
            }
        }

        fileName = outputDirectory + Log::time(true) + ".log";

        outFile.open(fileName, std::ios::app);
        if(!outFile.is_open())
        {
            fprintf(stderr, "Error while creating log file!\n");
            fflush(stderr);
            return;
        }

        outFile << Log::buildStartPrefix() << "\n";
    }

    outFile << content << "\n";
    // outFile.flush();
}

void Log::addSession(cstr content, bool newLine)
{
    Log::currentSession += content + (newLine ? "\n" : "");
}

// void Log::addSession(Log::Type logType, const QString &funName, const QString &message)
// {
//     Log::currentSession.addPart(logType, funName, message);
// }

std::string Log::Convert::vectorToString(std::vector<std::string> list)
{
    std::string str("[");
    for(const auto &i : list)
        str += "\"" + i + "\", ";
    return str + "\b\b]";
}
