#include <iostream>
#include "stacktrace.h"

#include "SimpleCompilerPipeline.h"

void handler()
{
    print_stacktrace();
}

int main(int argc, char *argv[]) {
    std::set_terminate(handler);

    // TODO: parse args properly (GNU getopt?)
    if (argc < 2) return 1;
    std::string inputFile = argv[argc - 1];
    std::string outputFile = "hello_world.z8";


    SimpleCompilerPipeline compiler;
    compiler.compile(inputFile, outputFile);

    return 0;
}

