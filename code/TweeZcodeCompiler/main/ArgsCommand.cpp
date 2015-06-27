//
// Created by manuel on 21.06.15.
//

#include "ArgsCommand.h"
#include <iostream>
#include <plog/Log.h>
#include <unistd.h>
#include <string>


ArgsCommand::ArgsCommand(int argc, char **argv) {
    this->argc = argc;
    this->argv = argv;

    int opt;
    this->debugInConsole = false;
    size_t findPosition;

    while ((opt = getopt(argc, argv, "i:o:a:d")) != -1) {
        switch (opt) {
            default:
                break;
            case 'i': //twee input file
                this->sourceFile = optarg;
                //std::cout   << "Input Source File:" << sourceFile;
                this->isAssembly = false;

                //define standard output file
                this->outputFile = sourceFile;
                findPosition = outputFile.find(".twee");
                if (findPosition == -1) {
                    std::cout << "Not a .twee Twee File"; //LOG_ERROR is not working here
                }

                outputFile.replace(findPosition, std::string(".twee").length(), ".z8");
                break;
            case 'o':
                this->outputFile = optarg;
                findPosition = this->outputFile.find(".z8");
                if (findPosition == -1) {
                    this->outputFile = this->outputFile + ".z8";
                }
                //std::cout  << "Output File:" << outputFile;

                break;
            case 'd':
                //std::cout << "Debug in Console is activated";
                this->debugInConsole = true;
                break;
            case '?':
                break;
            case 'a': //source file is Z-Assembly
                this->sourceFile = optarg;
                this->isAssembly = true;
                this->outputFile = this->sourceFile;

                //std::cout << "Input file is Z-Assembly";
                findPosition = outputFile.find(".zap");
                if (findPosition == -1) {
                    std::cout << "Not a .zap Z-Assembly File"; //LOG_ERROR is not working here
                }
                outputFile.replace(findPosition, std::string(".zap").length(), ".z8");
                break;
        }
    }
}


std::string ArgsCommand::getSourceFileName() const {
    return sourceFile;
}

std::string ArgsCommand::getOutputFileName() const {
    return outputFile;
}

bool ArgsCommand::sourceFileIsTwee() const {
    return !isAssembly;
}

bool ArgsCommand::sourceFileIsAssembly() const {
    return isAssembly;
}

bool ArgsCommand::isDebugInConsole() const {
    return debugInConsole;
}
