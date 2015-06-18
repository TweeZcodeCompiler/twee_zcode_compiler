#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include "stacktrace.h"
#include "SimpleCompilerPipeline.h"
#include "UserSideFormatter.h"

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

    //static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("Compiler_Log.txt");
    static plog::ConsoleAppender<plog::UserSideFormatter> consoleAppender; // Console Appender for user friendly output

    plog::init(plog::debug, "Compiler_Log.txt");
    plog::init(plog::error, &consoleAppender);

    LOG_DEBUG  << "Compiler started";
    LOG_ERROR  << "LOG_ERROR test";
    LOG_DEBUG  << "LOG_DEBUG test";
    SimpleCompilerPipeline compiler;
    compiler.compile(inputFile, outputFile);

    return 0;
}

