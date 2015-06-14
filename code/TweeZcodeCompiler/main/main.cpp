#include <iostream>

#include "SimpleCompilerPipeline.h"
#include "Logger.h"

int main(int argc, char *argv[]) {
    // TODO: parse args properly (GNU getopt?)
    if (argc < 2) return 1;
    std::string inputFile = argv[argc - 1];
    std::string outputFile = "hello_world.z8";

    Logger::init("Compiler_Log.txt");
    LOG_DEBUG  << "Compiler started";





    SimpleCompilerPipeline compiler;
    compiler.compile(inputFile, outputFile);

    return 0;
}

