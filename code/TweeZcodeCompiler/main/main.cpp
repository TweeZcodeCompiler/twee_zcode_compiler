#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <ParseException.h>
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

    remove("Compiler_Log.txt"); //reset Compiler Log file
    plog::init(plog::debug, "Compiler_Log.txt");
    plog::init(plog::error, &consoleAppender);

    LOG_DEBUG  << "Compiler started";

    TweeCompiler compiler;
    TweeZCodeCompilerPipeline pipeline;
    try {
        pipeline.compile(inputFile, outputFile, compiler);
    } catch (const AssemblyException& assemblyException) {
        std::cerr << "Invalid assembly statement at line " <<
                (assemblyException.lineNumber != 0 ? std::to_string(assemblyException.lineNumber) : "<unknown>") << ":" <<
                std::endl;

        std::cerr << "Reason: ";
        switch (assemblyException.errorType) {
            case AssemblyException::ErrorType::INVALID_INSTRUCTION:
                std::cerr << "Invalid instruction";
                break;
            case AssemblyException::ErrorType::INVALID_DIRECTIVE:
                std::cerr << "Invalid directive";
                break;
            case AssemblyException::ErrorType::INVALID_ROUTINE:
                std::cerr << "Invalid routine";
                break;
            case AssemblyException::ErrorType::INVALID_GLOBAL:
                std::cerr << "Invalid global variable";
                break;
            case AssemblyException::ErrorType::INVALID_LOCAL:
                std::cerr << "Invalid local variable";
                break;
            case AssemblyException::ErrorType::INVALID_LABEL:
                std::cerr << "Invalid label";
                if(const InvalidLabelException* invalidLabelException = dynamic_cast<const InvalidLabelException*>(&assemblyException)) {
                    std::cerr << " \"" << invalidLabelException->getLabelName() << "\"";
                }
                break;
            default:
                std::cerr << "unknown";
                break;
        }
        std::cerr << std::endl;

        if(assemblyException.line.compare("") != 0)
            std::cerr << ">>> " << assemblyException.line;
        return 1;
    } catch (const Twee::ParseException& parseException) {
        // TODO: display more info if possible
        std::cerr << "Failed parsing Twee File";
    }

    std::cout << inputFile << " compiled into --> " << outputFile << std::endl;

    return 0;
}

