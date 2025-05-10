#include <Log/Log.h>

#include "Program.h"

int main(int argc, char **argv){
    I("started");

    Program(argc, argv).run();

    return 0;
}
