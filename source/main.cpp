#include <Log/Log.h>

#include "Program.h"

int main(int argc, char **argv)
{
    R("Starting Application Code Files to Graph, version: %s\n", version);

    Program(argc, argv).run();

    return 0;
}
