#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include "stacktrace.h"
#include "TweeZCodeCompilerPipeline.h"
#include "UserSideFormatter.h"
#include "TweeCompiler.h"
#include "ArgsCommand.h"

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

int main(int argc, char **argv) {
    // install exception handler
   std::set_terminate(handler);


    //get cmdargs
    ArgsCommand argsCommand(argc,argv);
    // TODO: parse args properly (GNU getopt?)
    std::string inputFile = argsCommand.getSourceFileName();
    std::string outputFile = argsCommand.getOutputFileName();


   static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("Compiler_Log.txt");
    plog::ConsoleAppender<plog::UserSideFormatter> consoleAppender; // Console Appender for user friendly output

    std::string logFile = "Compiler_Log.txt";
    if(argsCommand.isDebugInConsole() == true)
    {

        plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
        std::cout << "Console log";
        plog::init(plog::debug,  &consoleAppender);
    }
    else
    {
        remove(logFile.c_str());
        plog::init(plog::debug, logFile.c_str());
    }
    plog::init(plog::error, &consoleAppender);


    LOG_DEBUG << "CMD-Args Summary";
    LOG_DEBUG << "Console-DEBUG-Output: " << argsCommand.isDebugInConsole();
    LOG_DEBUG << "Input-File:" << inputFile;
    LOG_DEBUG << "Twee File:" << argsCommand.sourceFileIsTwee();
    LOG_DEBUG << "Assembly File:" << argsCommand.sourceFileIsAssembly();
    LOG_DEBUG << "Output-file:" << outputFile;

    LOG_DEBUG  << "Compiler started";
    LOG_ERROR << "test";
     TweeCompiler compiler;
     TweeZCodeCompilerPipeline pipeline;
     pipeline.compile(inputFile, outputFile, compiler,argsCommand.sourceFileIsTwee());




    return 0;
}

