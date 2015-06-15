#include <iostream>
#include <plog/Log.h>
#include "stacktrace.h"
#include "SimpleCompilerPipeline.h"

/*
 * How to use Logger:
 *
    LOG_VERBOSE << "verbose";
    LOG_DEBUG << "debug";
    LOG_INFO << "info";
    LOG_WARNING << "warning";
    LOG_ERROR << "error";
    LOG_FATAL << "fatal";

    short forms

    LOGV << "verbose";
    LOGD << "debug";
    LOGI << "info";
    LOGW << "warning";
    LOGE << "error";
    LOGF << "fatal";
 */

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

    plog::init(plog::debug, "Compiler_Log.txt"); // initialize the logger
    LOG_DEBUG  << "Compiler started";

    SimpleCompilerPipeline compiler;
    compiler.compile(inputFile, outputFile);

    return 0;
}

