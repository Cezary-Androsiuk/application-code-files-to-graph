#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <cstdarg> // va_list
#include <filesystem>

// #include "LogSession.h"

#define SAPF(...) Log::asprintf(__VA_ARGS__) /* string as printf */



#define DISPLAY_OBJECT_LIFE_TIME true

/// Display Object Life Time Variable - Force
#define DOLTV_F(ptr, argsStr) {                                             \
std::string f_name(__FUNCTION__);                                       \
    if(f_name.empty())      f_name = "unknown action";                      \
    if(f_name[0] == '~')    f_name = "Destroying " + f_name;                \
    else                    f_name = "Creating " + f_name;                  \
    std::string qargsStr(argsStr);                                          \
    if(!qargsStr.empty()) qargsStr = "(" + qargsStr + ")";                  \
    DA(Log::Action::SaveSession, f_name + qargsStr + SAPF(": %p", ptr));    \
}

/// Display Object Life Time - Force
#define DOLT_F(ptr) DOLTV_F(ptr, "")

/// Display Object Life Time Variable
#define DOLTV(ptr, argsStr) if(DISPLAY_OBJECT_LIFE_TIME) { DOLTV_F(ptr, argsStr) }

/// Display Object Life Time
#define DOLT(ptr) DOLTV(ptr, "")




#define I(...) Log::info    (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__))
#define W(...) Log::warning (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__))
#define E(...) Log::error   (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__))
#define D(...) Log::debug   (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__))

#define IA(a, ...) Log::info    (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__), Log::Action(a));
#define WA(a, ...) Log::warning (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__), Log::Action(a));
#define EA(a, ...) Log::error   (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__), Log::Action(a));
#define DA(a, ...) Log::debug   (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__), Log::Action(a));




extern const char *outputDirectory;
#define EST_FUNCTION_LENGTH 70 // estimated function name length what will be reserved while creating log
#define SHORTER_FUNCTION_FILL_CHARACTER ' ' // characters that fills area before function name to fit estimated function name length
#define CONTENT_SPACE 10 // space between function name and content
#define CONTENT_SPACE_CHARACTER ' ' // characters that fills space between function name and content
#define SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT true // creates spaces between space: "x ........ y" instead of "x........y"

class Log
{
public:
    enum class Type{
        Info,
        Warning,
        Error,
        Debug,
    };
    static const char *logTypeToStr(Type type);

    enum Action{
        None = 0,
        Save = 1 << 0,
        Print = 1 << 1,
        Session = 1 << 2,
        SavePrint = Save | Print,
        SaveSession = Save | Session,
        PrintSession = Print | Session,
        All = Save | Print | Session,
    };
    static const char *logActionToStr(Action action);

    typedef const std::string &cstr;

    static constexpr Action actionForceHighest = Action::All; // set highest ( will be compared with & sign )
    static constexpr Action actionForceLowest = Action::None;  // set lowest ( will be compared with | sign )

    static void info(cstr func, cstr log, Action action = Action(Action::All));
    static void warning(cstr func, cstr log, Action action = Action(Action::All));
    static void error(cstr func, cstr log, Action action = Action(Action::All));
    static void debug(cstr func, cstr log, Action action = Action(Action::All));

    static std::string asprintf(const char *text, ...);
    static std::string asprintf(cstr text, ...);

private:
    static std::string time(bool simpleSeparators = false);
    static std::string buildPrefix(Type logType, cstr funName);
    static std::string buildStartPrefix();

    static void log(Type logType, cstr funName, cstr log, Action action = Action::All);
    static void safeLog(Type logType, cstr funName, cstr log, Action action = Action::All);
    static void print(cstr content);
    static void saveFile(cstr content);
    static void addSession(cstr content);
    // static void addSession(Type logType, cstr funName, cstr message);

public:
    class Convert{
    public:
        static std::string vectorToString(std::vector<std::string> list);
    };

    static bool firstLog;

    // static LogSession currentSession;
    static std::string currentSession;

    static std::string fileName;
    static std::ofstream outFile;
};

#endif // LOG_H
