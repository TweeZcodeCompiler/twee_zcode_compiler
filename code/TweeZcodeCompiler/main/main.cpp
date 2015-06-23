#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include "stacktrace.h"
#include "TweeZCodeCompilerPipeline.h"
#include "UserSideFormatter.h"
#include "TweeCompiler.h"

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
    // install exception handler
    std::set_terminate(handler);

    // TODO: parse args properly (GNU getopt?)
    if (argc < 2) return 1;
    std::string inputFile = argv[argc - 1];
    std::string outputFile = "hello_world.z8";

    //static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("Compiler_Log.txt");
    plog::ConsoleAppender<plog::UserSideFormatter> consoleAppender; // Console Appender for user friendly output

    if( remove( "Compiler_Log.txt" ) != 0 ){
        std::cerr << "Error deleting log file Compiler_Log.txt" << std::endl ;
    }
    else {
        std::cout << "Log file Compiler_Log.txt successfully deleted"  << std::endl ;
    }

    plog::init(plog::debug, "Compiler_Log.txt");
    plog::init(plog::error, &consoleAppender);

    LOG_DEBUG  << "Compiler started";
    LOG_ERROR  << "LOG_ERROR test";
    LOG_DEBUG  << "LOG_DEBUG test";
    TweeCompiler compiler;
    TweeZCodeCompilerPipeline pipeline;
    pipeline.compile(inputFile, outputFile, compiler);

    return 0;
}

